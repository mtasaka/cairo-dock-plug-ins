/*
** Login : <ctaf42@gmail.com>
** Started on  Fri Nov 30 05:31:31 2007 GESTES Cedric
** $Id$
**
** Copyright (C) 2007 GESTES Cedric
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef __SYSTRAY_INTERFACE__
#define  __SYSTRAY_INTERFACE__


#include <cairo-dock.h>


void cd_systray_build_dialog (void);

void cd_systray_build_systray (void);

void cd_systray_check_running (void);

void systray_on_keybinding_pull (const char *keystring, gpointer user_data);

void cd_systray_set_orientation (GtkOrientation o);


#endif

