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

#include <string.h>
#include <cairo-dock.h>

#include "applet-struct.h"
#include "applet-notifications.h"
#include "applet-config.h"


//\_________________ Here you have to get all your parameters from the conf file. Use the macros CD_CONFIG_GET_BOOLEAN, CD_CONFIG_GET_INTEGER, CD_CONFIG_GET_STRING, etc. myConfig has been reseted to 0 at this point. This function is called at the beginning of init and reload.
CD_APPLET_GET_CONFIG_BEGIN
	CD_CONFIG_RENAME_GROUP ("Configuration", "Drag and drop indicator");
	myConfig.fRotationSpeed = CD_CONFIG_GET_DOUBLE ("Drag and drop indicator", "rotation speed");
	myConfig.iSpeed = CD_CONFIG_GET_INTEGER ("Drag and drop indicator", "speed");
	myConfig.cDropIndicatorImageName = CD_CONFIG_GET_STRING ("Drag and drop indicator", "drop indicator");
	myConfig.cHoverIndicatorImageName = CD_CONFIG_GET_STRING ("Drag and drop indicator", "hover indicator");
CD_APPLET_GET_CONFIG_END


//\_________________ Here you have to free all ressources allocated for myConfig. This one will be reseted to 0 at the end of this function. This function is called right before you get the applet's config, and when your applet is stopped, in the end.
CD_APPLET_RESET_CONFIG_BEGIN
	g_free (myConfig.cDropIndicatorImageName);
	g_free (myConfig.cHoverIndicatorImageName);
CD_APPLET_RESET_CONFIG_END


//\_________________ Here you have to free all ressources allocated for myData. This one will be reseted to 0 at the end of this function. This function is called when your applet is stopped, in the very end.
static void _reset_data_on_one_dock (const gchar *cDockName, CairoDock *pDock, gpointer data)
{
	CDDropIndicatorData *pData = CD_APPLET_GET_MY_DOCK_DATA (pDock);
	if (pData == NULL)
		return ;
	g_free (pData);
	CD_APPLET_SET_MY_DOCK_DATA (pDock, NULL);
}
CD_APPLET_RESET_DATA_BEGIN
	gldi_docks_foreach ((GHFunc) _reset_data_on_one_dock, NULL);
	
	cd_drop_indicator_free_buffers ();
	
	if (myData.iBilinearGradationTexture != 0)
	{
		_cairo_dock_delete_texture (myData.iBilinearGradationTexture);
		myData.iBilinearGradationTexture = 0;
	}
CD_APPLET_RESET_DATA_END
