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
#include <math.h>
#include <string.h>

#include "applet-struct.h"
#include "applet-draw.h"


void cd_xkbd_update_icon (const gchar *cGroupName, const gchar *cShortGroupName, const gchar *cIndicatorName, gboolean bRedrawSurface)
{
	//g_print ("%s (%s;%s;%d)\n", __func__, cGroupName, cShortGroupName, bRedrawSurface);
	
	if (bRedrawSurface)
	{
		//\__________________ On sauvegarde l'ancienne surface/texture.
		if (myData.pOldSurface != NULL)
			cairo_surface_destroy (myData.pOldSurface);
		if (myData.iOldTexture != 0)
			_cairo_dock_delete_texture (myData.iOldTexture);
		myData.pOldSurface = myData.pCurrentSurface;
		myData.iOldTexture = myData.iCurrentTexture;
		myData.iOldTextWidth = myData.iCurrentTextWidth;
		myData.iOldTextHeight = myData.iCurrentTextHeight;
		
		//\__________________ On cree la nouvelle surface (la taille du texte peut avoir change).
		int iWidth, iHeight;
		CD_APPLET_GET_MY_ICON_EXTENT (&iWidth, &iHeight);
		if (iWidth <= 1 && iHeight <= 1)  // peut arriver au lancement en mode desklet.
		{
			myData.pCurrentSurface = NULL;
			myData.iCurrentTexture = 0;
			myData.iCurrentTextWidth = 0;
			myData.iCurrentTextHeight = 0;
			return;
		}
		double fMaxScale = cairo_dock_get_max_scale (myContainer);
		myData.pCurrentSurface = cairo_dock_create_surface_from_text_full (cShortGroupName,
			&myConfig.textDescription,
			fMaxScale,
			iWidth,
			&myData.iCurrentTextWidth, &myData.iCurrentTextHeight, NULL, NULL);
		myData.iCurrentTextWidth *= fMaxScale;
		myData.iCurrentTextHeight *= fMaxScale;
		if (g_bUseOpenGL)
		{
			myData.iCurrentTexture = cairo_dock_create_texture_from_surface (myData.pCurrentSurface);
		}
		
		//\__________________ On lance une transition entre ancienne et nouvelle surface/texture, ou on dessine direct.
		if (myConfig.iTransitionDuration != 0 && myData.pOldSurface != NULL)
		{
			CD_APPLET_SET_TRANSITION_ON_MY_ICON (cd_xkbd_render_step_cairo,
				cd_xkbd_render_step_opengl,
				g_bUseOpenGL,  // bFastPace : vite si opengl, lent si cairo.
				myConfig.iTransitionDuration,
				TRUE);  // bRemoveWhenFinished
		}
		else
		{
			if (CD_APPLET_MY_CONTAINER_IS_OPENGL)
			{
				CD_APPLET_START_DRAWING_MY_ICON_OR_RETURN ();
				cd_xkbd_render_step_opengl (myIcon, myApplet);	
				CD_APPLET_FINISH_DRAWING_MY_ICON;
			}
			else
			{
				cd_xkbd_render_step_cairo (myIcon, myApplet);
				CD_APPLET_UPDATE_REFLECT_ON_MY_ICON;
			}
			CD_APPLET_REDRAW_MY_ICON;
		}
		
		//\__________________ On met a jour le reste.
		CD_APPLET_SET_NAME_FOR_MY_ICON (cGroupName);
		CD_APPLET_SET_QUICK_INFO_ON_MY_ICON (cIndicatorName);
	}
	else
	{
		CD_APPLET_SET_QUICK_INFO_ON_MY_ICON (!cIndicatorName || *cIndicatorName == '\0' ? NULL : cIndicatorName);
		CD_APPLET_REDRAW_MY_ICON;
	}
}


gboolean cd_xkbd_render_step_opengl (Icon *pIcon, CairoDockModuleInstance *myApplet)
{
	CD_APPLET_ENTER;
	double f = CD_APPLET_GET_TRANSITION_FRACTION ();
	//g_print ("%s (%.2f; %.2fx%.2f)\n", __func__, f, myIcon->fWidth, myIcon->fHeight);
	
	int iWidth, iHeight;
	CD_APPLET_GET_MY_ICON_EXTENT (&iWidth, &iHeight);
	
	///cairo_dock_set_perspective_view (iWidth, iHeight);
	cairo_dock_set_perspective_view_for_icon (myIcon, myContainer);
	glScalef (1., -1., 1.);
	
	_cairo_dock_enable_texture ();
	_cairo_dock_set_blend_alpha ();
	_cairo_dock_set_alpha (1.);
	
	// fond
	if (myData.iBackgroundTexture != 0)
		cairo_dock_apply_texture_at_size (myData.iBackgroundTexture, iWidth, iHeight);
	
	double fTheta = - 45. + f * 90.;  // -45 -> 45
	glTranslatef (0., 0., - iWidth * sqrt(2)/2 * cos (fTheta/180.*G_PI));  // pour faire tenir le cube dans la fenetre.
	glEnable (GL_DEPTH_TEST);
	
	// image precedente.
	if (fTheta < 25 && myData.iOldTexture != 0)  // inutile de dessiner si elle est derriere l'image courante, par l'effet de perspective (en fait 22.5, mais bizarrement ca a l'air un peu trop tot).
	{
		glPushMatrix ();
		glRotatef (45. + fTheta, 0., 1., 0.);  // 0 -> 90
		glTranslatef (0., 0., (myData.iCurrentTextWidth ? myData.iCurrentTextWidth : iWidth)/2);
		cairo_dock_apply_texture_at_size (myData.iOldTexture, myData.iOldTextWidth, MIN (iHeight, myData.iOldTextHeight));
		glPopMatrix ();
	}
	
	// image courante a 90deg.
	glRotatef (45. + fTheta, 0., 1., 0.);  // 0 -> 90
	glTranslatef (- (myData.iOldTextWidth ? myData.iOldTextWidth : iWidth)/2, 0., 0.);
	
	glRotatef (-90., 0., 1., 0.);
	cairo_dock_apply_texture_at_size (myData.iCurrentTexture, myData.iCurrentTextWidth, MIN (iHeight, myData.iCurrentTextHeight));
	
	glDisable (GL_DEPTH_TEST);
	_cairo_dock_disable_texture ();
	
	if (myDock)
	{
		cairo_dock_set_ortho_view (myContainer);
	}
	
	CD_APPLET_LEAVE (TRUE);
}


gboolean cd_xkbd_render_step_cairo (Icon *pIcon, CairoDockModuleInstance *myApplet)
{
	CD_APPLET_ENTER;
	double f = CD_APPLET_GET_TRANSITION_FRACTION ();
	
	//g_print ("%s (%.2f)\n", __func__, f);
	int iWidth, iHeight;
	CD_APPLET_GET_MY_ICON_EXTENT (&iWidth, &iHeight);
	CD_APPLET_LEAVE_IF_FAIL (iHeight != 0, TRUE);
	
	cairo_dock_erase_cairo_context (myDrawContext);
	
	if (myData.pBackgroundSurface != NULL)
	{
		cairo_set_source_surface (
			myDrawContext,
			myData.pBackgroundSurface,
			0.,
			0.);
		cairo_paint (myDrawContext);
		cairo_dock_set_icon_surface_full (myDrawContext, myData.pBackgroundSurface, 1., 1., myIcon, myContainer);
	}
	
	double dx, dy, fScale;
	if (myData.pOldSurface != NULL && 1-f > .01)
	{
		dx = (iWidth - myData.iOldTextWidth)/2;
		dy = (iHeight - myData.iOldTextHeight)/2;
		if (dy < 0)  // peut arriver si une police de la taille de l'icone n'existe pas.
		{
			dy = 0;
			fScale = (double)iHeight / myData.iOldTextHeight;
			cairo_save (myDrawContext);
			cairo_scale (myDrawContext, 1., fScale);  // a priori la difference n'est pas enorme, on laisse donc remplit en largeur.
		}
		else
			fScale=0;
		cairo_set_source_surface (
			myDrawContext,
			myData.pOldSurface,
			dx,
			dy);
		cairo_paint_with_alpha (myDrawContext, 1-f);
		if (fScale != 0)
			cairo_restore (myDrawContext);
	}
	if (myData.pCurrentSurface != NULL)
	{
		dx = (iWidth - myData.iCurrentTextWidth)/2;
		dy = (iHeight - myData.iCurrentTextHeight)/2;
		if (dy < 0)
		{
			dy = 0;
			fScale = (double)iHeight / myData.iCurrentTextHeight;
			cairo_save (myDrawContext);
			cairo_scale (myDrawContext, 1., fScale);
		}
		else
			fScale=0;
		cairo_set_source_surface (
			myDrawContext,
			myData.pCurrentSurface,
			dx,
			dy);
		cairo_paint_with_alpha (myDrawContext, f);
		if (fScale != 0)
			cairo_restore (myDrawContext);
	}
	
	CD_APPLET_LEAVE (TRUE);
}
