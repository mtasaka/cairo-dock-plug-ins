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
#include <libxklavier/xklavier.h>

#include "applet-struct.h"
#include "applet-xklavier.h"
#include "applet-notifications.h"


//\___________ Define here the action to be taken when the user left-clicks on your icon or on its subdock or your desklet. The icon and the container that were clicked are available through the macros CD_APPLET_CLICKED_ICON and CD_APPLET_CLICKED_CONTAINER. CD_APPLET_CLICKED_ICON may be NULL if the user clicked in the container but out of icons.
CD_APPLET_ON_CLICK_BEGIN
	cd_xkbd_set_prev_next_group (+1);
CD_APPLET_ON_CLICK_END


//\___________ Define here the entries you want to add to the menu when the user right-clicks on your icon or on its subdock or your desklet. The icon and the container that were clicked are available through the macros CD_APPLET_CLICKED_ICON and CD_APPLET_CLICKED_CONTAINER. CD_APPLET_CLICKED_ICON may be NULL if the user clicked in the container but out of icons. The menu where you can add your entries is available throught the macro CD_APPLET_MY_MENU; you can add sub-menu to it if you want.
static void _select_group (GtkMenuItem *menu_item, gpointer *data)
{
	CD_APPLET_ENTER;
	int iNumGroup = GPOINTER_TO_INT (data);
	cd_xkbd_set_group (iNumGroup);
	CD_APPLET_LEAVE ();
}
CD_APPLET_ON_BUILD_MENU_BEGIN
	GtkWidget *pSubMenu = CD_APPLET_CREATE_MY_SUB_MENU ();
		CD_APPLET_ADD_ABOUT_IN_MENU (pSubMenu);
		
		const XklEngine *pEngine = xkl_engine_get_instance (cairo_dock_get_Xdisplay ());
		const gchar **pGroupNames = xkl_engine_get_groups_names (pEngine);
		int i;
		for (i = 0; pGroupNames[i] != NULL && *pGroupNames[i] != '-'; i ++)
		{
			CD_APPLET_ADD_IN_MENU_WITH_DATA (pGroupNames[i], _select_group, pSubMenu, GINT_TO_POINTER (i));
		}
CD_APPLET_ON_BUILD_MENU_END


CD_APPLET_ON_SCROLL_BEGIN
	cd_xkbd_set_prev_next_group (CD_APPLET_SCROLL_UP ? +1 : -1);
CD_APPLET_ON_SCROLL_END
