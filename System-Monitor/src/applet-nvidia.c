/**
* This file is a part of the Cairo-Dock project
*
* Copyright : (C) see the 'copyright' file.
* E-mail    : see the 'copyright' file.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 3
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <fcntl.h>
#include <unistd.h>
#include <unistd.h>

#include "applet-struct.h"
#include "applet-nvidia.h"

void cd_sysmonitor_get_nvidia_data (GldiModuleInstance *myApplet)
{
	gchar *cCommand = g_strdup_printf ("nvidia-settings -q GPUCoreTemp -t");
	gchar *cResult = cairo_dock_launch_command_sync (cCommand);
	g_free (cCommand);
	
	gint iGpuTemp = 0;
	if (cResult != NULL)
		iGpuTemp = atoi (cResult);
	
	if (iGpuTemp == 0) {
		cd_warning("nVidia : couldn't acquire GPU temperature\n is 'nvidia-settings' installed on your system and its version >= 1.0 ?");
		myData.bAcquisitionOK = FALSE;
	}
	else {
		myData.iGPUTemp = iGpuTemp;
	}
	
	if (myData.iGPUTemp <= myConfig.iLowerLimit)
		myData.fGpuTempPercent = 0;
	else if (myData.iGPUTemp >= myConfig.iUpperLimit)
		myData.fGpuTempPercent = 100.;
	else
		myData.fGpuTempPercent = 100. * (myData.iGPUTemp - myConfig.iLowerLimit) / (myConfig.iUpperLimit - myConfig.iLowerLimit);
	if (fabs (myData.fGpuTempPercent - myData.fPrevGpuTempPercent) > 1)
	{
		myData.fPrevGpuTempPercent = myData.fGpuTempPercent;
		myData.bNeedsUpdate = TRUE;
	}
}


static void _get_nvidia_info (GldiModuleInstance *myApplet)
{
	gchar *cCommand = g_strdup_printf ("bash %s/nvidia-config", MY_APPLET_SHARE_DATA_DIR);
	gchar *cResult = cairo_dock_launch_command_sync (cCommand);
	g_free (cCommand);
	if (cResult == NULL || *cResult == '\n')
	{
		myData.cGPUName = g_strdup ("none");
		return ;
	}
	
	gchar **cInfopipesList = g_strsplit (cResult, "\n", -1);
	g_free (cResult);
	gchar *cOneInfopipe;
	gint i=0;
	
	g_free (myData.cGPUName);
	myData.cGPUName = NULL;
	g_free (myData.cDriverVersion);
	myData.cDriverVersion = NULL;
	
	for (i = 0; cInfopipesList[i] != NULL; i ++) {
		cOneInfopipe = cInfopipesList[i];
		if (*cOneInfopipe == '\0')
			continue;
		
		if ((i == 0) && (strcmp (cOneInfopipe,"nvidia") == 0)) {
			cd_warning ("problem while getting nVidia GPU temperature.");
			g_strfreev (cInfopipesList);
			return ;
		}
		else {
			if (i == 0) {
				gchar *str = g_strstr_len (cOneInfopipe, strlen (cOneInfopipe), "version");
				if (str != NULL) {
					str += 7;
					while (*str == ' ')
						str ++;
					gchar *str2 = strchr (str, ' ');
					if (str2 != NULL)
						*str2 = '\0';
					int iMajorVersion=0, iMinorVersion=0, iMicroVersion=0;
					cairo_dock_get_version_from_string (str, &iMajorVersion, &iMinorVersion, &iMicroVersion);
					/*if (iMajorVersion == 0 || (iMajorVersion == 1 && iMinorVersion < 0)) { /// A confirmer ...
						myData.bSettingsTooOld == TRUE;
						cd_warning ("Attention : your nvidia-settings's version is too old (%d.%d.%d)", iMajorVersion, iMinorVersion, iMicroVersion);
						break ;
					}*/
				}
			}
			else if (i == 1) { //GPU Name
				myData.cGPUName = g_strdup (cOneInfopipe);
				gchar *str = strchr (myData.cGPUName, ')');
				if (str != NULL)
					*str = '\0';
			}
			else if (i == 2) { //Video Ram
				myData.iVideoRam = atoi (cOneInfopipe);
				myData.iVideoRam = myData.iVideoRam >> 10;  // passage en Mo.
			}
			else if (i == 3) { //Driver Version
				myData.cDriverVersion = g_strdup (cOneInfopipe);
			}
		}
	}
	
	cd_debug ("nVidia %s %dMB %sV %d°C", myData.cGPUName, myData.iVideoRam, myData.cDriverVersion, myData.iGPUTemp);
	
	g_strfreev (cInfopipesList);
}

void cd_sysmonitor_get_nivdia_info (GldiModuleInstance *myApplet, GString *pInfo)
{
	if (myData.cGPUName == NULL)  // nvidia-config n'a encore jamais ete appele.
		_get_nvidia_info (myApplet);
	
	if (myData.cGPUName && strcmp (myData.cGPUName, "none") != 0)  // nvidia-config est passe avec succes.
	{
		if (!myConfig.bShowNvidia)
			cd_sysmonitor_get_nvidia_data (myApplet);  // le thread ne passe pas par la => pas de conflit.
		
		g_string_append_printf (pInfo, "\n%s: %s\n  %s: %d%s \n  %s: %s\n  %s: %d°C",
			D_("GPU model"), myData.cGPUName,
			D_("Video Ram"), myData.iVideoRam, D_("Mb"),
			D_("Driver Version"), myData.cDriverVersion,
			D_("Core Temperature"), myData.iGPUTemp);
	}
}



void cd_nvidia_alert (GldiModuleInstance *myApplet)
{
	if (myData.bAlerted || ! myConfig.bAlert)
		return;
	
	gldi_dialogs_remove_on_icon (myIcon);
	gldi_dialog_show_temporary_with_icon_printf (D_("Alert! Graphic Card core temperature has reached %d°C"), myIcon, myContainer, 4e3, MY_APPLET_SHARE_DATA_DIR"/"MY_APPLET_ICON_FILE, myData.iGPUTemp);
	
	if (myConfig.bAlertSound)
		cairo_dock_play_sound (myConfig.cSoundPath);
	
	myData.bAlerted = TRUE;
}
