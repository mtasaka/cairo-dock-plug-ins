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
#include <string.h>
#include <glib/gi18n.h>

#include "applet-struct.h"
#include "applet-wifi.h"
#include "applet-netspeed.h"
#include "applet-connections.h"
#include "applet-menu.h"
#include "applet-draw.h"
#include "applet-notifications.h"


CD_APPLET_ON_CLICK_BEGIN
	if (myData.bDbusConnection && myData.bWirelessExt)
	{
		GtkWidget *pMenu = cd_NetworkMonitor_build_menu_with_access_points ();
		if (pMenu)
			CD_APPLET_POPUP_MENU_ON_MY_ICON (pMenu);
	}
	else
	{
		gldi_dialogs_remove_on_icon (myIcon);
		cd_NetworkMonitor_bubble();
	}
CD_APPLET_ON_CLICK_END


static void _cd_NetworkMonitor_activate_network (GtkMenuItem *menu_item, gpointer data)
{
	guint iState = cairo_dock_dbus_get_property_as_uint (myData.dbus_proxy_NM_prop, "org.freedesktop.NetworkManager", "State");
	gboolean bSleep;
	bSleep = (iState != 1);  // 1 = NM_STATE_ASLEEP
	
	dbus_g_proxy_call_no_reply (myData.dbus_proxy_NM, "Sleep",
		G_TYPE_BOOLEAN, bSleep,
		G_TYPE_INVALID);
}
static void _cd_NetworkMonitor_activate_wifi (GtkMenuItem *menu_item, gpointer data)
{
	gboolean bWirelessEnabled = cairo_dock_dbus_get_property_as_boolean (myData.dbus_proxy_NM_prop, "org.freedesktop.NetworkManager", "WirelessEnabled");
	
	cairo_dock_dbus_set_boolean_property (myData.dbus_proxy_NM_prop, "org.freedesktop.NetworkManager", "WirelessEnabled", ! bWirelessEnabled);
}
static void cd_NetworkMonitor_recheck_wireless_extension (GtkMenuItem *menu_item, gpointer data)
{
	gldi_task_stop (myData.netSpeed.pTask);
	gldi_task_launch (myData.netSpeed.pTask);
}
static void _cd_NetworkMonitor_show_config (GtkMenuItem *menu_item, gpointer data)
{
	if (myConfig.cWifiConfigCommand != NULL) {
		cairo_dock_launch_command (myConfig.cWifiConfigCommand);
	}
	else
	{
		cairo_dock_launch_command ("nm-connection-editor");  // network-admin n'est plus present depuis Intrepid, et nm-connection-editor marche aussi sous KDE.
	}
}
CD_APPLET_ON_BUILD_MENU_BEGIN
	if (! myData.bWirelessExt && myData.bDbusConnection)
		CD_APPLET_ADD_IN_MENU (D_("Check for Wireless Extension"), cd_NetworkMonitor_recheck_wireless_extension, CD_APPLET_MY_MENU);
	CD_APPLET_ADD_IN_MENU (D_("Network Administration"), _cd_NetworkMonitor_show_config, CD_APPLET_MY_MENU);
	if (myData.bDbusConnection)
	{
		guint iState = cairo_dock_dbus_get_property_as_uint (myData.dbus_proxy_NM_prop, "org.freedesktop.NetworkManager", "State");
		CD_APPLET_ADD_IN_MENU (iState == 1 ? D_("Activate network") : D_("Deactivate network"), _cd_NetworkMonitor_activate_network, CD_APPLET_MY_MENU);
		if (myData.bWirelessExt)  /// remplacer ca par un booleen qui dit si un device wifi existe...
		{
			gboolean bWirelessEnabled = cairo_dock_dbus_get_property_as_boolean (myData.dbus_proxy_NM_prop, "org.freedesktop.NetworkManager", "WirelessEnabled");
			CD_APPLET_ADD_IN_MENU (bWirelessEnabled ? D_("Deactivate wifi") : D_("Activate wifi"), _cd_NetworkMonitor_activate_wifi, CD_APPLET_MY_MENU);
		}
	}
CD_APPLET_ON_BUILD_MENU_END

/*
CD_APPLET_ON_MIDDLE_CLICK_BEGIN
	/// trouver une action utile ...
CD_APPLET_ON_MIDDLE_CLICK_END
*/

CD_APPLET_ON_SCROLL_BEGIN
	//\____________ On change le mode.
	myConfig.bModeWifi = ! myConfig.bModeWifi;
	
	//\____________ On l'ecrit en conf.
	cairo_dock_update_conf_file (CD_APPLET_MY_CONF_FILE,
		G_TYPE_INT, "Configuration", "mode", myConfig.bModeWifi,
		G_TYPE_INVALID);
	
	//\____________ On lance la nouvelle tache.
	if (! myData.bDbusConnection)
	{
		if (myConfig.bModeWifi)
		{
			cd_netmonitor_launch_wifi_task (myApplet);
		}
		else
		{
			cd_netmonitor_launch_netspeed_task (myApplet);
		}
	}
CD_APPLET_ON_SCROLL_END
