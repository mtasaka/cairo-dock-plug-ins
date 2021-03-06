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
#include <glib/gi18n.h>
#include <glib.h>
#include <glib/gprintf.h>

#include "applet-struct.h"
#include "applet-notifications.h"
#include "applet-wifi.h"
#include "applet-netspeed.h"

#define NETSPEED_DATA_PIPE "/proc/net/dev"


// Prend un debit en octet par seconde et le transforme en une chaine de la forme : xxx yB/s
static void cd_netspeed_formatRate (GldiModuleInstance *myApplet, unsigned long long rate, gchar* debit) {
	int smallRate;
	
	if (rate <= 0)
	{
		if (myDesklet)
			g_sprintf(debit, "0 %s/s", D_("B"));
		else
			g_sprintf(debit, "0");
	}
	else if (rate < 1024)
	{
		smallRate = rate;
		if (myDesklet)
			g_sprintf(debit, "%i %s/s", smallRate, D_("B"));
		else
			g_sprintf(debit, "%iB", smallRate);
	}
	else if (rate < (1<<20))
	{
		smallRate = rate >> 10;
		if (myDesklet)
			g_sprintf(debit, "%i %s/s", smallRate, D_("KB"));
		else
			g_sprintf(debit, "%iK", smallRate);
	}
	else if (rate < (1<<30))
	{
		smallRate = rate >> 20;
		if (myDesklet)
			g_sprintf(debit, "%i %s/s", smallRate, D_("MB"));
		else
			g_sprintf(debit, "%iM", smallRate);
	}
	else if (rate < ((unsigned long long)1<<40))
	{
		smallRate = rate >> 30;
		if (myDesklet)
			g_sprintf(debit, "%i %s/s", smallRate, D_("GB"));
		else
			g_sprintf(debit, "%iG", smallRate);
	}
	else  // c'est vraiment pour dire qu'on est exhaustif :-)
	{
		smallRate = rate >> 40;
		if (myDesklet)
			g_sprintf(debit, "%i %s/s", smallRate, D_("TB"));
		else
			g_sprintf(debit, "%iT", smallRate);
	}
}


void cd_netspeed_get_data (GldiModuleInstance *myApplet)
{
	double fTimeElapsed = gldi_task_get_elapsed_time (myData.netSpeed.pTask);
	
	gchar *cContent = NULL;
	gsize length=0;
	GError *erreur = NULL;
	g_file_get_contents (NETSPEED_DATA_PIPE, &cContent, &length, &erreur);
	if (erreur != NULL)
	{
		cd_warning("NetSpeed : %s", erreur->message);
		g_error_free(erreur);
		erreur = NULL;
		myData.netSpeed._bAcquisitionOK = FALSE;
	}
	else if (cContent && *cContent != '\0')
	{
		int iNumLine = 1;
		gchar *tmp = cContent;
		long long int iReceivedBytes, iTransmittedBytes;
		do
		{
			if (iNumLine > 2)  // first 2 lines are the names of the fields (next lines are the various interfaces, in any order, the loopback is not always the first one).
			{
				while (*tmp == ' ')  // on saute les espaces.
					tmp ++;
				
				if (!myConfig.cInterface || (strncmp (tmp, myConfig.cInterface, myConfig.iStringLen) == 0 && *(tmp+myConfig.iStringLen) == ':'))  // c'est l'interface qu'on veut.
				{
					if (myConfig.cInterface)
					{
						tmp += myConfig.iStringLen+1;  // on saute le ':' avec.
					}
					else
					{
						gchar *str = strchr (tmp, ':');
						if (str)
							tmp = str+1;
					}
					while (*tmp == ' ')  // on saute les espaces.
						tmp ++;
					iReceivedBytes = atoll (tmp);
					
					int i = 0;
					for (i = 0; i < 8; i ++)  // on saute les 8 valeurs suivantes.
					{
						while (*tmp != ' ')  // saute le chiffre courant.
							tmp ++;
						while (*tmp == ' ')  // saute les espaces.
							tmp ++;
					}
					iTransmittedBytes = atoll (tmp);
					
					if (myConfig.cInterface || iReceivedBytes != 0 || iTransmittedBytes != 0)  // c'est l'interface voulue ou c'est une interface avec du debit => on prend.
					{
						if (myData.netSpeed._bInitialized && fTimeElapsed > .1)  // la 1ere iteration on ne peut pas calculer le debit.
						{
							myData.netSpeed._iDownloadSpeed = (iReceivedBytes - myData.netSpeed._iReceivedBytes) / fTimeElapsed;
							myData.netSpeed._iUploadSpeed = (iTransmittedBytes - myData.netSpeed._iTransmittedBytes) / fTimeElapsed;
						}
						
						myData.netSpeed._iReceivedBytes = iReceivedBytes;
						myData.netSpeed._iTransmittedBytes = iTransmittedBytes;
						break ;
					}
				}
			}
			tmp = strchr (tmp, '\n');
			if (tmp == NULL)
				break;
			tmp ++;
			iNumLine ++;
		}
		while (1);
		myData.netSpeed._bAcquisitionOK = (tmp != NULL);
		g_free (cContent);
		if (! myData.netSpeed._bInitialized)
			myData.netSpeed._bInitialized = TRUE;
	}
	else
		myData.netSpeed._bAcquisitionOK = FALSE;
}

gboolean cd_netspeed_update_from_data (GldiModuleInstance *myApplet)
{
	static double s_fValues[CD_NETSPEED_NB_MAX_VALUES];
	static gchar s_upRateFormatted[11];
	static gchar s_downRateFormatted[11];
	
	// recopie des infos qu'on veut pouvoir exploiter en dehors de la tache periodique.
	myData.netSpeed.bAcquisitionOK = myData.netSpeed._bAcquisitionOK;
	myData.netSpeed.iReceivedBytes = myData.netSpeed._iReceivedBytes;
	myData.netSpeed.iTransmittedBytes = myData.netSpeed._iTransmittedBytes;
	
	if ( ! myData.netSpeed._bAcquisitionOK)
	{
		/*if (myConfig.iInfoDisplay == CAIRO_DOCK_INFO_ON_LABEL)
			CD_APPLET_SET_NAME_FOR_MY_ICON (myConfig.defaultTitle);
		else if (myConfig.iInfoDisplay == CAIRO_DOCK_INFO_ON_ICON)*/
			CD_APPLET_SET_QUICK_INFO_ON_MY_ICON ("N/A");
		
		memset (s_fValues, 0, sizeof (s_fValues));
		CD_APPLET_RENDER_NEW_DATA_ON_MY_ICON (s_fValues);
		
		gldi_task_downgrade_frequency (myData.netSpeed.pTask);
	}
	else
	{
		gldi_task_set_normal_frequency (myData.netSpeed.pTask);
		
		if (! myData.netSpeed._bInitialized)
		{
			//if (myConfig.iInfoDisplay == CAIRO_DOCK_INFO_ON_ICON)
				CD_APPLET_SET_QUICK_INFO_ON_MY_ICON (myDock ? "..." : D_("Loading"));
			memset (s_fValues, 0, sizeof (s_fValues));
			CD_APPLET_RENDER_NEW_DATA_ON_MY_ICON (s_fValues);
		}
		else
		{
			//if (myConfig.iInfoDisplay != CAIRO_DOCK_INFO_NONE)
			{
				cd_netspeed_formatRate (myApplet, myData.netSpeed._iUploadSpeed, s_upRateFormatted);
				cd_netspeed_formatRate (myApplet, myData.netSpeed._iDownloadSpeed, s_downRateFormatted);
				//if (myConfig.iInfoDisplay == CAIRO_DOCK_INFO_ON_ICON)
				{
					CD_APPLET_SET_QUICK_INFO_ON_MY_ICON_PRINTF ("↓%s\n↑%s", s_downRateFormatted, s_upRateFormatted);
				}
				/*else
				{
					CD_APPLET_SET_NAME_FOR_MY_ICON_PRINTF ("↓%s\n↑%s", s_downRateFormatted, s_upRateFormatted);
				}*/
			}
			
			if(myData.netSpeed._iUploadSpeed > myData.netSpeed._iMaxUpRate) {
				myData.netSpeed._iMaxUpRate = myData.netSpeed._iUploadSpeed;
			}
			if(myData.netSpeed._iDownloadSpeed > myData.netSpeed._iMaxDownRate) {
				myData.netSpeed._iMaxDownRate = myData.netSpeed._iDownloadSpeed;
			}
			
			double fUpValue, fDownValue;
			if (myData.netSpeed._iMaxUpRate != 0)
				fUpValue = (double) myData.netSpeed._iUploadSpeed / myData.netSpeed._iMaxUpRate;
			else
				fUpValue = 0.;
			if (myData.netSpeed._iMaxDownRate != 0)
				fDownValue = (double) myData.netSpeed._iDownloadSpeed / myData.netSpeed._iMaxDownRate;
			else
				fDownValue = 0.;
			
			s_fValues[0] = fDownValue;
			s_fValues[1] = fUpValue;
			CD_APPLET_RENDER_NEW_DATA_ON_MY_ICON (s_fValues);
		}
	}
	return TRUE;
}


void cd_netmonitor_launch_netspeed_task (GldiModuleInstance *myApplet)
{
	cd_netmonitor_free_wifi_task (myApplet);
	
	if (myData.netSpeed.pTask == NULL)  // la tache n'existe pas, on la cree et on la lance.
	{
		myData.netSpeed.pTask = gldi_task_new (myConfig.iNetspeedCheckInterval,
			(GldiGetDataAsyncFunc) cd_netspeed_get_data,
			(GldiUpdateSyncFunc) cd_netspeed_update_from_data,
			myApplet);
		gldi_task_launch (myData.netSpeed.pTask);
	}
	else  // la tache existe, on la relance immediatement, avec la nouvelle frequence eventuellement.
	{
		gldi_task_change_frequency_and_relaunch (myData.netSpeed.pTask, myConfig.iNetspeedCheckInterval);
	}
}

void cd_netmonitor_free_netspeed_task (GldiModuleInstance *myApplet)
{
	if (myData.netSpeed.pTask != NULL)
	{
		gldi_task_free (myData.netSpeed.pTask);
		myData.netSpeed.pTask = NULL;
	}
}


GList *cd_netmonitor_get_wireless_interfaces (void)
{
	//Inter-|sta|  Quality       |  Discarded packets
	//face |tus|link level noise| nwid crypt  misc
	//eth2: f0   15.  24.    4.   181     0     0
	gchar *cWireless = NULL;
	gsize length=0;
	gchar **cWirelessInterfaces = NULL;
	g_file_get_contents ("/proc/net/wireless", &cWireless, &length, NULL);
	if (cWireless == NULL)
		return NULL;
	
	cWirelessInterfaces = g_strsplit (cWireless, "\n", -1);
	g_free (cWireless);
	if (cWirelessInterfaces == NULL)
		return NULL;
	
	GList *pWirelessInterfaces = NULL;
	int i;
	for (i = 0; cWirelessInterfaces[i] != NULL; i ++)
	{
		if (i < 2)
			continue;
		gchar *str = cWirelessInterfaces[i];
		while (*str == ' ')
			str ++;
		gchar *str2 = strchr (str, ' ');
		if (str2)
			*str2 = '\0';
		pWirelessInterfaces = g_list_prepend (pWirelessInterfaces, g_strdup (str));
	}
	g_strfreev (cWirelessInterfaces);
	
	return pWirelessInterfaces;
}

GList *cd_netmonitor_get_available_interfaces (GList *pWirelessInterfaces)
{
	// on recupere le contenu du pipe.
	GList *pList = NULL;
	gchar *cContent = NULL;
	gsize length=0;
	GError *erreur = NULL;
	g_file_get_contents (NETSPEED_DATA_PIPE, &cContent, &length, &erreur);
	if (erreur != NULL)
	{
		cd_warning("NetSpeed : %s", erreur->message);
		g_error_free(erreur);
		return NULL;
	}
	g_return_val_if_fail (cContent != NULL, NULL);
	
	// on extrait les interfaces de chaque ligne.
	GList *wi;
	gboolean bIsWireless;
	int iNumLine = 1;
	gchar *tmp = cContent, *str;
	gchar *cInterface;
	do
	{
		if (iNumLine > 3 && *tmp != '\0')  // les 2 premieres lignes sont les noms des champs, la 3eme est la loopback.
		{
			while (*tmp == ' ')  // on saute les espaces.
				tmp ++;
			
			str = strchr (tmp, ':');
			if (str)
			{
				*str = '\0';
				bIsWireless = FALSE;
				// on cherche si c'est du sans fil.
				for (wi = pWirelessInterfaces; wi != NULL; wi = wi->next)
				{
					if (strcmp (wi->data, tmp) == 0)
					{
						bIsWireless = TRUE;
						break;
					}
				}
				
				if (bIsWireless)
					cInterface = g_strdup_printf ("%s (wifi)", tmp);
				else if (pWirelessInterfaces != NULL)
					cInterface = g_strdup_printf ("%s (ethernet)", tmp);
				else
					cInterface = g_strdup (tmp);
				
				pList = g_list_prepend (pList, cInterface);
				tmp = str+1;
			}
		}
		tmp = strchr (tmp, '\n');
		if (tmp == NULL)
			break;
		tmp ++;
		iNumLine ++;
	}
	while (1);
	g_free (cContent);
	
	return pList;
}

int cd_netmonitor_check_interface (const gchar *cInterface)
{
	GList *pList = cd_netmonitor_get_available_interfaces (NULL);
	gpointer s = g_list_find_custom (pList, cInterface, (GCompareFunc)strcmp);
	g_list_foreach (pList, (GFunc)g_free, NULL);
	g_list_free (pList);
	if (s == NULL)
		return 0;
	
	GList *pWirelessInterfaces = cd_netmonitor_get_wireless_interfaces ();
	s = g_list_find_custom (pList, cInterface, (GCompareFunc)strcmp);
	g_list_foreach (pWirelessInterfaces, (GFunc)g_free, NULL);
	g_list_free (pWirelessInterfaces);
	if (s == NULL)
		return 1;
	else
		return 2;
}
