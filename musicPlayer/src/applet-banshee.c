/******************************************************************************

This file is a part of the cairo-dock program, 
released under the terms of the GNU General Public License.

Written by Yann SLADEK (for any bug report, please mail me to mav@cairo-dock.org)
Fabrice Rey (fabounet@users.berlios.de)
Rémy Robertson (changfu@cairo-dock.org)

******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <glib/gi18n.h>
#include <cairo-dock.h>


#include "applet-struct.h"
#include "applet-musicplayer.h"
#include "applet-dbus.h"

#include "applet-draw.h"
#include "applet-banshee.h"

CD_APPLET_INCLUDE_MY_VARS


//Les Fonctions
void cd_banshee_free_data (void) //Permet de libéré la mémoire prise par notre controleur
{
	musicplayer_dbus_disconnect_from_bus();
}

/* Controle du lecteur */
void cd_banshee_control (MyPlayerControl pControl, char* nothing) //Permet d'effectuer les actions de bases sur le lecteur
{ 
	gchar *cCommand = NULL;
	
	/* Conseil de ChangFu pour redetecter le titre à coup sûr */
	g_free (myData.cRawTitle);
	myData.cRawTitle = NULL;
		
	switch (pControl) {
		case PLAYER_PREVIOUS :
			cCommand = myData.DBus_commands.previous;
		break;
		
		case PLAYER_PLAY_PAUSE :
			cCommand = myData.DBus_commands.play_pause;
		break;

		case PLAYER_NEXT :
			cCommand = myData.DBus_commands.next;
		break;
		
		/*case PLAYER_ENQUEUE :
			// A faire
		break;*/
		
		default :
			return;
	}
	
	cd_debug ("MP : Handeler banshee : will use '%s'", cCommand);
	cd_musicplayer_dbus_command(cCommand);
}

/* Permet de renseigner l'applet des fonctions supportées par le lecteur */
gboolean cd_banshee_ask_control (MyPlayerControl pControl) 
{
	switch (pControl) {
		case PLAYER_PREVIOUS :
			return TRUE;
		break;
		
		case PLAYER_PLAY_PAUSE :
			return TRUE;		
		break;

		case PLAYER_NEXT :
			return TRUE;
		break;
		
		default :
			return FALSE;
	}
}

/* Fonction de connexion au bus de banshee */
void cd_banshee_acquisition (void) 
{
	cd_musicplayer_check_dbus_connection();	
}


/* Fonction de lecture des infos */
void cd_banshee_read_data (void) 
{
	if (myData.dbus_enable)
	{
		if (myData.opening)
		{
			cd_musicplayer_getStatus_integer(); // On récupère l'état de la lecture (play/pause/stop)
			if (myData.pPlayingStatus == PLAYER_PLAYING)
			{
				cd_musicplayer_getSongInfos(); // On récupère toutes les infos de la piste en cours
				cd_musicplayer_banshee_getTime();
				cd_musicplayer_getCoverPath();
			}	
		}
		else
		{
			cd_debug("MP : lecteur non ouvert");
			myData.pPlayingStatus = PLAYER_NONE;	
		}
	}
	else
	{
		cd_debug("MP : Impossible d'accéder au bus");
		myData.pPlayingStatus = PLAYER_BROKEN;
	}
	
}

void cd_banshee_load_dbus_commands (void)
{
	cd_debug("MP : On charge les commande pour Banshee");
	myData.DBus_commands.service = "org.gnome.Banshee";
	myData.DBus_commands.path = "/org/gnome/Banshee/Player";
	myData.DBus_commands.interface = "org.gnome.Banshee.Core";
	myData.DBus_commands.play = "Play";
	myData.DBus_commands.pause = "Pause";
	myData.DBus_commands.play_pause = "TogglePlaying";
	myData.DBus_commands.next = "Next";
	myData.DBus_commands.previous = "Previous";
	myData.DBus_commands.get_title = "GetPlayingTitle";
	myData.DBus_commands.get_artist = "GetPlayingArtist";
	myData.DBus_commands.get_album = "GetPlayingAlbum";
	myData.DBus_commands.get_cover_path = "GetPlayingCoverUri";
	myData.DBus_commands.get_status = "GetPlayingStatus";
	myData.DBus_commands.duration = "GetPlayingDuration";
	myData.DBus_commands.current_position = "GetPlayingPosition";
	cd_debug("MP : Chargement des fonctions DBus effectué");
	return;
	
}

void cd_musicplayer_banshee_getTime (void)
{
	/* Récupération de la position actuelle */
	myData.iPreviousCurrentTime = myData.iCurrentTime;
	myData.iCurrentTime = cd_musicplayer_getCurPos_integer();
	
	/* Récupération du temps total */
	myData.iSongLength = cd_musicplayer_getLength_integer();
	
	cd_debug("MP : Temps écoulé : %i", myData.iCurrentTime);
	cd_debug("MP : Temps total de la chanson : %i", myData.iSongLength);
}



void cd_musicplayer_register_banshee_handeler (void) { //On enregistre notre lecteur
	MusicPlayerHandeler *pBanshee = g_new0 (MusicPlayerHandeler, 1);
	pBanshee->acquisition = cd_banshee_acquisition;
	pBanshee->read_data = cd_banshee_read_data;
	pBanshee->free_data = cd_banshee_free_data;
	pBanshee->configure = cd_banshee_load_dbus_commands; //Cette fonction permettera de préparé le controleur
	//Pour les lecteurs utilisants dbus, c'est elle qui connectera le dock aux services des lecteurs etc..
	pBanshee->control = cd_banshee_control;
	pBanshee->ask_control = cd_banshee_ask_control;
	pBanshee->appclass = g_strdup("Banshee"); //Toujours g_strdup sinon l'applet plante au free_handler
	pBanshee->name = g_strdup("Banshee");
	cd_musicplayer_register_my_handeler(pBanshee,"Banshee");
}
