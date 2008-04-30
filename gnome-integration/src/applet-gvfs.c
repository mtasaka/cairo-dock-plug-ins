/******************************************************************************

This file is a part of the cairo-dock program, 
released under the terms of the GNU General Public License.

Written by Fabrice Rey (for any bug report, please mail me to fabounet@users.berlios.de)
Some inspiration was taken from the "xdg" project.

******************************************************************************/
#include <string.h>

#include <glib.h>
#include <gio/gio.h>

#include "applet-gvfs.h"

static GHashTable *s_hMonitorHandleTable = NULL;


static void _vfs_backend_free_monitor_data (gpointer *data)
{
	if (data != NULL)
	{
		GFileMonitor *pHandle = data[2];
		g_file_monitor_cancel (pHandle);  // le GFileMonitor est-il libere lors du g_file_monitor_cancel () ?
		g_free (data);
	}
}

gboolean init_vfs_backend (void)
{
	if (s_hMonitorHandleTable != NULL)
		g_hash_table_destroy (s_hMonitorHandleTable);
	
	s_hMonitorHandleTable = g_hash_table_new_full (g_str_hash,
		g_str_equal,
		g_free,
		(GDestroyNotify) _vfs_backend_free_monitor_data);
	
	GVfs *vfs = g_vfs_get_default ();
	return (vfs != NULL && g_vfs_is_active (vfs));  // utile ?
}

void stop_vfs_backend (void)
{
	if (s_hMonitorHandleTable != NULL)
	{
		g_hash_table_destroy (s_hMonitorHandleTable);
		s_hMonitorHandleTable = NULL;
	}
}



static gboolean file_manager_get_file_info_from_desktop_link (const gchar *cBaseURI, gchar **cName, gchar **cURI, gchar **cIconName, gboolean *bIsDirectory, int *iVolumeID)
{
	cd_message ("%s (%s)", __func__, cBaseURI);
	GError *erreur = NULL;
	
	GFile *pFile = g_file_new_for_uri (cBaseURI);
	gsize length = 0;
	gchar *cFileData = NULL, *etag_out = NULL;
	g_file_load_contents (pFile,
		NULL,
		&cFileData,
		&length,
		&etag_out,
		&erreur);
	if (erreur != NULL)
	{
		cd_warning ("Attention : %s", erreur->message);
		g_error_free (erreur);
		return FALSE;
	}
	g_print ("cFileData : %s\n", cFileData);
	
	GKeyFile *pKeyFile = g_key_file_new ();
	g_key_file_load_from_data (pKeyFile,
		cFileData,
		-1,
		G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS,
		&erreur);
	g_free (cFileData);
	if (erreur != NULL)
	{
		cd_warning ("Attention : %s", erreur->message);
		g_error_free (erreur);
		return FALSE;
	}

	gchar *cType = g_key_file_get_value (pKeyFile, "Desktop Entry", "Type", NULL);
	g_print ("  cType : %s\n", cType);
	if (strncmp (cType, "Link", 4) != 0 && strncmp (cType, "FSDevice", 8) != 0)
	{
		g_free(cType);
		g_key_file_free (pKeyFile);
		return FALSE;
	}
	g_free(cType);

	*cName = g_key_file_get_string (pKeyFile, "Desktop Entry", "Name", NULL);
	
	*cURI = g_key_file_get_string (pKeyFile, "Desktop Entry", "URL", NULL);
	
	*cIconName = g_key_file_get_string (pKeyFile, "Desktop Entry", "Icon", NULL);	
	
	*iVolumeID = g_key_file_get_integer (pKeyFile, "Desktop Entry", "X-Gnome-Drive", NULL);	
	
	*bIsDirectory = TRUE;
	
	g_key_file_free (pKeyFile);
	return TRUE;
}

static gchar *_cd_get_icon_path (GIcon *pIcon)
{
	gchar *cIconPath= NULL;
	if (G_IS_THEMED_ICON (pIcon))
	{
		const gchar * const *cFileNames = g_themed_icon_get_names (G_THEMED_ICON (pIcon));
		//g_print ("icones possibles : %s\n", g_strjoinv (":", (gchar **) cFileNames));
		int i;
		for (i = 0; cFileNames[i] != NULL && cIconPath == NULL; i ++)
		{
			//g_print (" une icone possible est : %s\n", cFileNames[i]);
			cIconPath = cairo_dock_search_icon_s_path (cFileNames[i]);
			//g_print ("  chemin trouve : %s\n", cIconPath);
		}
	}
	else if (G_IS_FILE_ICON (pIcon))
	{
		GFile *pFile = g_file_icon_get_file (G_FILE_ICON (pIcon));
		gchar *cIconPath = g_file_get_basename (pFile);
		//g_print (" file_icon => %s\n", cIconPath);
	}
	return cIconPath;
}

void vfs_backend_get_file_info (const gchar *cBaseURI, gchar **cName, gchar **cURI, gchar **cIconName, gboolean *bIsDirectory, int *iVolumeID, double *fOrder, CairoDockFMSortType iSortType)
{
	g_return_if_fail (cBaseURI != NULL);
	cd_message ("%s (%s)", __func__, cBaseURI);
	
	gchar *cFullURI;
	if (strncmp (cBaseURI, "x-nautilus-desktop://", 21) == 0)
	{
		gchar *cNautilusFile = g_strdup_printf ("computer://%s", cBaseURI+21);
		if (g_str_has_suffix (cBaseURI, ".volume"))
		{
			cNautilusFile[strlen(cNautilusFile)-7] = '\0';
			cFullURI = g_strdup_printf ("%s.drive", cNautilusFile);
			g_free (cNautilusFile);
			gchar **cSplit = g_strsplit (cFullURI, "%20", -1);
			if (cSplit != NULL && cSplit[0] != NULL)
			{
				int i = 0;
				g_free (cFullURI);
				GString *sURI = g_string_new (cSplit[0]);
				for (i = 1; cSplit[i] != NULL; i ++)
				{
					cd_message ("%d) %s\n", i, cSplit[i]);
					g_string_append_printf (sURI, "%%2520%s", cSplit[i]);
				}
				cFullURI = sURI->str;
				g_string_free (sURI, FALSE);
			}
			g_strfreev (cSplit);
		}
	}
	else
	{
		if (*cBaseURI == '/')
			cFullURI = g_strconcat ("file://", cBaseURI, NULL);
		else
			cFullURI = g_strdup (cBaseURI);
	}
	cd_message (" -> cFullURI : %s", cFullURI);
	
	GFile *pFile = g_file_new_for_uri (cFullURI);
	GError *erreur = NULL;
	gchar *cQuery = g_strconcat (G_FILE_ATTRIBUTE_STANDARD_TYPE, ",",
		G_FILE_ATTRIBUTE_STANDARD_SIZE, ",",
		G_FILE_ATTRIBUTE_TIME_MODIFIED, ",",
		G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE, ",",
		G_FILE_ATTRIBUTE_STANDARD_NAME, ",",
		G_FILE_ATTRIBUTE_STANDARD_ICON, ",",
		G_FILE_ATTRIBUTE_MOUNTABLE_UNIX_DEVICE, NULL);
	GFileInfo *pFileInfo = g_file_query_info (pFile,
		cQuery,
		G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
		NULL,
		&erreur);
	g_free (cQuery);
	g_object_unref (pFile);
	if (erreur != NULL)
	{
		cd_warning ("Attention : %s", erreur->message);
		g_error_free (erreur);
		return ;
	}
	
	*cURI = cFullURI;
	*cName = g_strdup (g_file_info_get_name (pFileInfo));
	const gchar *cMimeType = g_file_info_get_content_type (pFileInfo);
	GFileType iFileType = g_file_info_get_file_type (pFileInfo);
	
	if (iSortType == CAIRO_DOCK_FM_SORT_BY_DATE)
	{
		GTimeVal t;
		g_file_info_get_modification_time (pFileInfo, &t);
		*fOrder = t.tv_sec;
	}
	else if (iSortType == CAIRO_DOCK_FM_SORT_BY_SIZE)
		*fOrder = g_file_info_get_size (pFileInfo);
	else if (iSortType == CAIRO_DOCK_FM_SORT_BY_TYPE)
		*fOrder = (cMimeType != NULL ? *((int *) cMimeType) : 0);
	else
		*fOrder = 0;
	
	*bIsDirectory = (iFileType == G_FILE_TYPE_DIRECTORY);
	g_print (" => '%s' (mime:%s ; bIsDirectory:%d)\n", *cName, cMimeType, *bIsDirectory);
	
	if (cMimeType == NULL)
	{
		file_manager_get_file_info_from_desktop_link (cFullURI, cName, cURI, cIconName, bIsDirectory, iVolumeID);
		*fOrder = 0;
		return ;
	}
	
	*cIconName = NULL;
	if (cMimeType != NULL && strncmp (cMimeType, "image", 5) == 0)
	{
		gchar *cHostname = NULL;
		GError *erreur = NULL;
		gchar *cFilePath = g_filename_from_uri (cBaseURI, &cHostname, &erreur);
		if (erreur != NULL)
		{
			g_error_free (erreur);
		}
		else if (cHostname == NULL || strcmp (cHostname, "localhost") == 0)  // on ne recupere la vignette que sur les fichiers locaux.
		{
			*cIconName = g_strdup (cFilePath);
			cairo_dock_remove_html_spaces (*cIconName);
		}
		g_free (cHostname);
	}
	if (*cIconName == NULL)
	{
		GIcon *pSystemIcon = g_file_info_get_icon (pFileInfo);
		if (pSystemIcon != NULL)
		{
			*cIconName = _cd_get_icon_path (pSystemIcon);
		}
	}
	g_print ("cIconName : %s\n", *cIconName);
	
	*iVolumeID = g_file_info_get_attribute_uint32 (pFileInfo, G_FILE_ATTRIBUTE_MOUNTABLE_UNIX_DEVICE);
	g_print ("ID : %d\n", *iVolumeID);
	g_object_unref (pFileInfo);
}

static Icon *_cd_get_icon_for_volume (GVolume *pVolume, GMount *pMount)
{
	Icon *pNewIcon = NULL;
	GIcon *pIcon;
	GFile *pRootDir;
	
	if (pVolume != NULL)
		pMount = g_volume_get_mount (pVolume);
	else if (pMount == NULL)
		return NULL;
	
	if (pMount != NULL)  // ce volume est monte.
	{
		pNewIcon = g_new0 (Icon, 1);
		pNewIcon->acName = g_mount_get_name (pMount);
		
		pRootDir = g_mount_get_root (pMount);
		pNewIcon->acCommand = g_file_get_uri (pRootDir);
		g_object_unref (pRootDir);
		
		pIcon = g_mount_get_icon (pMount);
		pNewIcon->acFileName = _cd_get_icon_path (pIcon);
		g_object_unref (pIcon);
		
		pNewIcon->iVolumeID = pMount;
		g_object_unref (pMount);
	}
	else  // ce volume est demonte, on le montre quand meme (l'automount peut etre off).
	{
		pNewIcon = g_new0 (Icon, 1);
		pNewIcon->acName = g_volume_get_name (pVolume);
		
		pIcon = g_volume_get_icon (pVolume);
		pNewIcon->acFileName = _cd_get_icon_path (pIcon);
		g_object_unref (pIcon);
		
		pNewIcon->acCommand = g_strdup (pNewIcon->acName);
		pNewIcon->iVolumeID = pVolume;
	}
	//pNewIcon->iVolumeID = 1;
	pNewIcon->cBaseURI = g_strdup (pNewIcon->acCommand);
	g_print (" => %s\n", pNewIcon->acCommand);
	return pNewIcon;
}

GList *vfs_backend_list_volumes (void)
{
	GVolumeMonitor *pVolumeMonitor = g_volume_monitor_get ();
	GList *pIconsList = NULL;
	Icon *pNewIcon;
	
	//\___________________ On chope les disques connectes (lecteur de CD/disquette/etc) et on liste leurs volumes.
	GList *pDrivesList = g_volume_monitor_get_connected_drives (pVolumeMonitor);
	GList *pAssociatedVolumes;
	GList *dl, *av;
	GDrive *pDrive;
	GVolume *pVolume;
	for (dl = pDrivesList; dl != NULL; dl = dl->next)
	{
		pDrive = dl->data;
		g_print ("drive '%s'\n", g_drive_get_name  (pDrive));
		
		pAssociatedVolumes = g_drive_get_volumes (pDrive);
		if (pAssociatedVolumes != NULL)
		{
			for (av = pAssociatedVolumes; av != NULL; av = av->next)
			{
				g_print (" + volume '%s'\n", g_volume_get_name  (pVolume));
				pVolume = av->data;
				pNewIcon = _cd_get_icon_for_volume (pVolume, NULL);
				if (pNewIcon != NULL)
					pIconsList = g_list_prepend (pIconsList, pNewIcon);
				g_object_unref (pVolume);
			}
			g_list_free (pAssociatedVolumes);
		}
		else  // le disque n'a aucun volume montable
		{
			g_print ("  le disque n'a aucun volume montable\n");
			/*if (g_drive_is_media_removable (pDrive) && ! g_drive_is_media_check_automatic (pDrive))
			{
				g_drive_get_icon (pDrive);
				g_drive_get_name (pDrive);
			}*/
		}
		g_object_unref (pDrive);
	}
	g_list_free (pDrivesList);

	//\___________________ On chope les volumes qui ne sont pas associes a un disque.
	GList *pVolumesList = g_volume_monitor_get_volumes (pVolumeMonitor);
	GList *v;
	for (v = pVolumesList; v != NULL; v = v->next)
	{
		pVolume = v->data;
		g_print ("volume '%s'\n", g_volume_get_name  (pVolume));
		pDrive = g_volume_get_drive (pVolume);
		if (pDrive != NULL)  // on l'a deja liste dans la 1ere boucle.
		{
			g_print ("  drive '%s' est deja liste\n", g_drive_get_name (pDrive));
			g_object_unref (pDrive);
		}
		else
		{
			g_print (" + volume '%s'\n", g_volume_get_name  (pVolume));
			if (pNewIcon != NULL)
				pNewIcon = _cd_get_icon_for_volume (pVolume, NULL);
			pIconsList = g_list_prepend (pIconsList, pNewIcon);
		}
		g_object_unref (pVolume);
	}
	g_list_free (pVolumesList);

	//\___________________ On chope les points de montage qui n'ont pas de volumes. (montage de mtab, ftp, etc)
	GList *pMountsList = g_volume_monitor_get_mounts (pVolumeMonitor);
	GMount *pMount;
	GList *m;
	for (m = pMountsList; m != NULL; m = m->next)
	{
		pMount = m->data;
		g_print ("mount '%s'\n", g_mount_get_name (pMount));
		pVolume = g_mount_get_volume (pMount);
		if (pVolume != NULL)  // on l'a deja liste precedemment.
		{
			g_print ("volume '%s' est deja liste\n", g_volume_get_name  (pVolume));
			g_object_unref (pVolume);
		}
		else
		{
			g_print ("+ volume '%s'\n", g_volume_get_name  (pVolume));
			if (pNewIcon != NULL)
				pNewIcon = _cd_get_icon_for_volume (NULL, pMount);
			pIconsList = g_list_prepend (pIconsList, pNewIcon);
		}
		g_object_unref (pMount);
	}
	g_list_free (pMountsList);
	
	return pIconsList;
}


GList *vfs_backend_list_directory (const gchar *cBaseURI, CairoDockFMSortType iSortType, int iNewIconsType, gboolean bListHiddenFiles, gchar **cFullURI)
{
	g_return_val_if_fail (cBaseURI != NULL, NULL);
	cd_message ("%s (%s)", __func__, cBaseURI);
	
	gchar *cURI;
	if (strcmp (cBaseURI, CAIRO_DOCK_FM_VFS_ROOT) == 0)
	{
		cURI = g_strdup ("computer:/");
		///*cFullURI = cURI;
		///return vfs_backend_list_volumes ();
	}
	else if (strcmp (cBaseURI, CAIRO_DOCK_FM_NETWORK) == 0)
		cURI = g_strdup ("network:/");
	else
		cURI = (*cBaseURI == '/' ? g_strconcat ("file://", cBaseURI, NULL) : g_strdup (cBaseURI));
	*cFullURI = cURI;
	
	GFile *pFile = g_file_new_for_uri (cURI);
	GError *erreur = NULL;
	gchar *cAttributes = g_strconcat (G_FILE_ATTRIBUTE_STANDARD_TYPE, ",",
		G_FILE_ATTRIBUTE_STANDARD_SIZE, ",",
		G_FILE_ATTRIBUTE_TIME_MODIFIED, ",",
		G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE, ",",
		G_FILE_ATTRIBUTE_STANDARD_NAME, ",",
		G_FILE_ATTRIBUTE_STANDARD_IS_HIDDEN, ",",
		G_FILE_ATTRIBUTE_STANDARD_ICON, ",",
		G_FILE_ATTRIBUTE_STANDARD_TARGET_URI, ",",
		G_FILE_ATTRIBUTE_UNIX_RDEV, ",",
		G_FILE_ATTRIBUTE_MOUNTABLE_UNIX_DEVICE, NULL);
	GFileEnumerator *pFileEnum = g_file_enumerate_children (pFile,
		cAttributes,
		G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
		NULL,
		&erreur);
	g_free (cAttributes);
	g_object_unref (pFile);
	if (erreur != NULL)
	{
		cd_warning ("Attention : %s", erreur->message);
		g_error_free (erreur);
		return NULL;
	}
	
	GList *pIconList = NULL;
	Icon *icon;
	GFileInfo *pFileInfo;
	do
	{
		pFileInfo = g_file_enumerator_next_file (pFileEnum, NULL, &erreur);
		if (erreur != NULL)
		{
			cd_warning ("Attention : %s", erreur->message);
			g_error_free (erreur);
			erreur = NULL;
		}
		else
		{
			if (pFileInfo == NULL)
				break ;
			
			gboolean bIsHidden = g_file_info_get_is_hidden (pFileInfo);
			if (bListHiddenFiles || ! bIsHidden)
			{
				GFileType iFileType = g_file_info_get_file_type (pFileInfo);
				const gchar *cName = g_file_info_get_name (pFileInfo);
				GIcon *pSystemIcon = g_file_info_get_icon (pFileInfo);
				if (pSystemIcon == NULL)
				{
					g_print ("AUCUNE ICONE\n");
					continue;
				}
				const gchar *cMimeType = g_file_info_get_content_type (pFileInfo);
				
				gchar *cFileURI = g_strconcat (*cFullURI, "/", cName, NULL);
				cd_message (" + cFileURI : %s (mime:%s)", cFileURI, cMimeType);
				icon = g_new0 (Icon, 1);
				icon->iType = iNewIconsType;
				icon->cBaseURI = cFileURI;
				
				if (iFileType == G_FILE_TYPE_MOUNTABLE)
				{
					cFileURI = g_strdup (g_file_info_get_attribute_string (pFileInfo, G_FILE_ATTRIBUTE_STANDARD_TARGET_URI));
					g_print ("c'est un point de montage correspondant a %s (ID:%d)\n", cFileURI, g_file_info_get_attribute_uint32 (pFileInfo, G_FILE_ATTRIBUTE_UNIX_RDEV));
					
					GFile *file = g_file_new_for_uri (cFileURI);
					GMount *pMount = g_file_find_enclosing_mount (file, NULL, NULL);
					if (pMount != NULL)
					{
						g_print ("un GMount existe ! (%s)\n", g_mount_get_name (pMount));
						cName = g_mount_get_name (pMount);
						
						GVolume *volume = g_mount_get_volume (pMount);
						g_print ("  (volume %s)\n", g_volume_get_name (volume));
						GDrive *drive = g_mount_get_drive (pMount);
						g_print ("  (drive %s)\n", g_drive_get_name (drive));
						
						pSystemIcon = g_mount_get_icon (pMount);
					}
					/*if (cFileURI == NULL)
					{
						g_free (icon);
						continue;
					}*/
					/*gboolean bIsDirectory = FALSE;
					file_manager_get_file_info_from_desktop_link (cFileURI, &icon->acName, &icon->acCommand, &icon->acFileName, &bIsDirectory, &icon->iVolumeID);
					cd_message ("  bIsDirectory : %d; iVolumeID : %d; acFileName : %s", bIsDirectory, icon->iVolumeID, icon->acFileName);*/
					icon->iVolumeID = 1;
				}
				//else
				{
					icon->acCommand = g_strdup (icon->cBaseURI);
					icon->acName = g_strdup (cName);
					icon->acFileName = NULL;
					if (cMimeType != NULL && strncmp (cMimeType, "image", 5) == 0)  // && strncmp (cFileURI, "file://", 7) == 0
					{
						gchar *cHostname = NULL;
						gchar *cFilePath = g_filename_from_uri (cFileURI, &cHostname, &erreur);
						if (erreur != NULL)
						{
							g_error_free (erreur);
							erreur = NULL;
						}
						else if (cHostname == NULL || strcmp (cHostname, "localhost") == 0)  // on ne recupere la vignette que sur les fichiers locaux.
						{
							icon->acFileName = g_strdup (cFilePath);
							cairo_dock_remove_html_spaces (icon->acFileName);
						}
						g_free (cHostname);
						g_free (cFilePath);
					}
					if (icon->acFileName == NULL)
					{
						icon->acFileName = _cd_get_icon_path (pSystemIcon);
						g_print ("icon->acFileName : %s\n", icon->acFileName);
					}
				}
				//icon->iVolumeID = g_file_info_get_attribute_uint32 (pFileInfo, G_FILE_ATTRIBUTE_MOUNTABLE_UNIX_DEVICE);
				//g_print ("ID : %d\n", icon->iVolumeID);
				icon->iVolumeID = (iFileType == G_FILE_TYPE_MOUNTABLE);
				
				if (iSortType == CAIRO_DOCK_FM_SORT_BY_SIZE)
					icon->fOrder = g_file_info_get_size (pFileInfo);
				else if (iSortType == CAIRO_DOCK_FM_SORT_BY_DATE)
				{
					GTimeVal t;
					g_file_info_get_modification_time (pFileInfo, &t);
					icon->fOrder = t.tv_sec;
				}
				else if (iSortType == CAIRO_DOCK_FM_SORT_BY_TYPE)
					icon->fOrder = (cMimeType != NULL ? *((int *) cMimeType) : 0);
				pIconList = g_list_prepend (pIconList, icon);
			}
		}
	} while (TRUE);  // 'g_file_enumerator_close' est appelee lors du dernier 'g_file_enumerator_next_file'.
	
	if (iSortType == CAIRO_DOCK_FM_SORT_BY_NAME)
		pIconList = cairo_dock_sort_icons_by_name (pIconList);
	else
		pIconList = cairo_dock_sort_icons_by_order (pIconList);
	
	return pIconList;
}



void vfs_backend_launch_uri (const gchar *cURI)
{
	g_return_if_fail (cURI != NULL);
	GError *erreur = NULL;
	gchar *cFullURI = (*cURI == '/' ? g_strconcat ("file://", cURI, NULL) : g_strdup (cURI));
	
	g_print ("%s (%s)\n", __func__, cFullURI);
	gboolean bSuccess = g_app_info_launch_default_for_uri (cFullURI,
		NULL,
		&erreur);
	g_free (cFullURI);
	if (erreur != NULL)
	{
		cd_warning ("Attention : couldn't get file info for '%s' [%s]", cURI, erreur->message);
		g_error_free (erreur);
	}
}

gchar *_cd_find_target_uri (gchar *cBaseURI)
{
	GError *erreur = NULL;
	/*GFile *pFile = g_file_new_for_uri (cBaseURI);
	GFileInfo *pFileInfo = g_file_query_info (pFile,
		G_FILE_ATTRIBUTE_STANDARD_TARGET_URI,
		G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
		NULL,
		&erreur);
	g_object_unref (pFile);
	if (erreur != NULL)
	{
		cd_warning ("Attention : %s", erreur->message);
		g_error_free (erreur);
		return NULL;
	}
	const gchar *cTargetURI = g_file_info_get_attribute_string (pFileInfo, G_FILE_ATTRIBUTE_STANDARD_TARGET_URI);*/
	GFile *pFile = g_file_new_for_uri ("computer:/");
	GFileEnumerator *pFileEnum = g_file_enumerate_children (pFile,
		G_FILE_ATTRIBUTE_STANDARD_TARGET_URI","G_FILE_ATTRIBUTE_STANDARD_NAME,
		G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
		NULL,
		&erreur);
	g_object_unref (pFile);
	if (erreur != NULL)
	{
		cd_warning ("Attention : %s", erreur->message);
		g_error_free (erreur);
		return NULL;
	}
	
	gchar *cBaseName = g_path_get_basename (cBaseURI);
	gchar *cTargetURI = NULL;
	const gchar *cName;
	GFileInfo *pFileInfo;
	do
	{
		pFileInfo = g_file_enumerator_next_file (pFileEnum, NULL, &erreur);
		if (erreur != NULL)
		{
			cd_warning ("Attention : %s", erreur->message);
			g_error_free (erreur);
			erreur = NULL;
		}
		else
		{
			if (pFileInfo == NULL)
				break ;
			cName = g_file_info_get_name (pFileInfo);
			if (cTargetURI == NULL && cName != NULL && strcmp (cName, cBaseName) == 0)
			{
				cTargetURI = g_strdup (g_file_info_get_attribute_string (pFileInfo, G_FILE_ATTRIBUTE_STANDARD_TARGET_URI));
			}
		}
	} while (TRUE);  // 'g_file_enumerator_close' est appelee lors du dernier 'g_file_enumerator_next_file'.
	g_free (cBaseName);
	return cTargetURI;
}

GMount *_cd_find_mount_from_uri (const gchar *cURI)
{
	gchar *cTargetURI = _cd_find_target_uri (cURI);
	
	if (cTargetURI != NULL)
	{
		g_print ("  pointe sur %s\n", cTargetURI);
		GFile *file = g_file_new_for_uri (cTargetURI);
		g_free (cTargetURI);
		GMount *pMount = g_file_find_enclosing_mount (file, NULL, NULL);
		return pMount;
	}
	return NULL;
}

gchar *vfs_backend_is_mounted (const gchar *cURI, gboolean *bIsMounted)
{
	cd_message ("%s (%s)", __func__, cURI);
	GMount *pMount = _cd_find_mount_from_uri (cURI);
	if (pMount != NULL)
		*bIsMounted = TRUE;
	else
		*bIsMounted = FALSE;
	
	return NULL;
}

static void _vfs_backend_mount_callback (gpointer pObject, GAsyncResult *res, gpointer *data)
//static void _vfs_backend_mount_callback (gboolean succeeded, char *error, char *detailed_error, gpointer *data)
{
	cd_message ("%s ()", __func__);
	
	CairoDockFMMountCallback pCallback = data[0];
	
	GError *erreur = NULL;
	gboolean bSuccess;
	if (GPOINTER_TO_INT (data[1]) == 1)
		bSuccess = (g_file_mount_mountable_finish (G_FILE (pObject), res, &erreur) != NULL);
	else if (GPOINTER_TO_INT (data[1]) == 0)
		bSuccess = g_mount_unmount_finish (G_MOUNT (pObject), res, &erreur);
	else
		bSuccess = g_mount_eject_finish (G_MOUNT (pObject), res, &erreur);
	if (erreur != NULL)
	{
		cd_warning ("Attention : %s", erreur->message);
		g_error_free (erreur);
	}
	
	g_print ("(un)mount fini -> %d\n", bSuccess);
	pCallback (GPOINTER_TO_INT (data[1]) == 1, bSuccess, data[2], data[3], data[4]);
	g_free (data[2]);
	g_object_unref (pObject);
	g_free (data);
}

void vfs_backend_mount (const gchar *cURI, int iVolumeID, CairoDockFMMountCallback pCallback, Icon *icon, CairoDock *pDock)
{
	g_return_if_fail (iVolumeID > 0);
	cd_message ("%s (%s)", __func__, cURI);
	
	gchar *cTargetURI = _cd_find_target_uri (cURI);
	cTargetURI = g_strdup (cURI);
	GFile *pFile = g_file_new_for_uri (cTargetURI);
	
	gpointer *data2 = g_new (gpointer, 5);
	data2[0] = pCallback;
	data2[1] = GINT_TO_POINTER (1);
	data2[2] = g_path_get_basename (cTargetURI);
	data2[3] = icon;
	data2[4] = pDock;
	g_file_mount_mountable  (pFile,
		G_MOUNT_MOUNT_NONE,
		NULL,
		NULL,
		(GAsyncReadyCallback) _vfs_backend_mount_callback,
		data2);
	g_free (cTargetURI);
}

void vfs_backend_unmount (const gchar *cURI, int iVolumeID, CairoDockFMMountCallback pCallback, Icon *icon, CairoDock *pDock)
{
	g_return_if_fail (cURI != NULL);
	cd_message ("%s (%s)", __func__, cURI);
	
	GMount *pMount = _cd_find_mount_from_uri (cURI);
	if (pMount == NULL)
	{
		return ;
	}
	
	if ( ! g_mount_can_unmount (pMount))
		return ;
	
	gboolean bCanEject = g_mount_can_eject (pMount);
	gboolean bCanUnmount = g_mount_can_unmount (pMount);
	g_print ("eject:%d / unmount:%d\n", bCanEject, bCanUnmount);
	if (! bCanEject && ! bCanUnmount)
		return ;
	
	gpointer *data2 = g_new (gpointer, 5);
	data2[0] = pCallback;
	data2[1] = GINT_TO_POINTER (bCanEject ? 2 : 0);
	data2[2] = g_mount_get_name (pMount);
	data2[3] = icon;
	data2[4] = pDock;
	if (bCanEject)
		g_mount_eject (pMount,
			G_MOUNT_UNMOUNT_NONE ,
			NULL,
			(GAsyncReadyCallback) _vfs_backend_mount_callback,
			data2);
	else
		g_mount_unmount (pMount,
			G_MOUNT_UNMOUNT_NONE ,
			NULL,
			(GAsyncReadyCallback) _vfs_backend_mount_callback,
			data2);
}


void _on_monitor_changed (GFileMonitor *monitor,
	GFile *file,
	GFile *other_file,
	GFileMonitorEvent event_type,
	gpointer  *data)
{
	CairoDockFMMonitorCallback pCallback = data[0];
	gpointer user_data = data[1];
	cd_message ("%s (%d , data : %x)", __func__, event_type, user_data);
	
	CairoDockFMEventType iEventType;
	switch (event_type)
	{
		case G_FILE_MONITOR_EVENT_CHANGED :
			iEventType = CAIRO_DOCK_FILE_MODIFIED;
		break;
		
		case G_FILE_MONITOR_EVENT_DELETED :
			iEventType = CAIRO_DOCK_FILE_DELETED;
		break;
		
		case G_FILE_MONITOR_EVENT_CREATED :
			iEventType = CAIRO_DOCK_FILE_CREATED;
		break;
		
		default :
		return ;
	}
	gchar *cURI = g_file_get_uri (file);
	pCallback (iEventType, cURI, user_data);
	g_free (cURI);
}


void vfs_backend_add_monitor (const gchar *cURI, gboolean bDirectory, CairoDockFMMonitorCallback pCallback, gpointer user_data)
{
	g_return_if_fail (cURI != NULL);
	GError *erreur = NULL;
	GFileMonitor *pMonitor;
	GFile *pFile = (*cURI == '/' ? g_file_new_for_path (cURI) : g_file_new_for_uri (cURI));
	if (bDirectory)
		pMonitor = g_file_monitor_directory (pFile,
			G_FILE_MONITOR_WATCH_MOUNTS,
			NULL,
			&erreur);
	else
		pMonitor = g_file_monitor_file (pFile,
			G_FILE_MONITOR_WATCH_MOUNTS,
			NULL,
			&erreur);
	g_object_unref (pFile);
	if (erreur != NULL)
	{
		cd_warning ("Attention : couldn't add monitor on '%s' (%d) [%s]", cURI, bDirectory, erreur->message);
		g_error_free (erreur);
		return ;
	}
	
	gpointer *data = g_new0 (gpointer, 3);
	data[0] = pCallback;
	data[1] = user_data;
	data[2] = pMonitor;
	g_signal_connect (G_OBJECT (pMonitor), "changed", G_CALLBACK (_on_monitor_changed), data);
	
	g_hash_table_insert (s_hMonitorHandleTable, g_strdup (cURI), data);
	cd_message (">>> moniteur ajoute sur %s (%x)", cURI, user_data);
}

void vfs_backend_remove_monitor (const gchar *cURI)
{
	if (cURI != NULL)
	{
		cd_message (">>> moniteur supprime sur %s", cURI);
		g_hash_table_remove (s_hMonitorHandleTable, cURI);
	}
}



gboolean vfs_backend_delete_file (const gchar *cURI)
{
	g_return_val_if_fail (cURI != NULL, FALSE);
	GFile *pFile = (*cURI == '/' ? g_file_new_for_path (cURI) : g_file_new_for_uri (cURI));
	
	GError *erreur = NULL;
	gboolean bSuccess = g_file_delete (pFile, NULL, &erreur);
	if (erreur != NULL)
	{
		cd_warning ("Attention : %s", erreur->message);
		g_error_free (erreur);
	}
	
	g_object_unref (pFile);
	return bSuccess;
}

gboolean vfs_backend_rename_file (const gchar *cOldURI, const gchar *cNewName)
{
	g_return_val_if_fail (cOldURI != NULL, FALSE);
	GFile *pOldFile = (*cOldURI == '/' ? g_file_new_for_path (cOldURI) : g_file_new_for_uri (cOldURI));
	GError *erreur = NULL;
	GFile *pNewFile = g_file_set_display_name (pOldFile, cNewName, NULL, &erreur);
	if (erreur != NULL)
	{
		cd_warning ("Attention : %s", erreur->message);
		g_error_free (erreur);
	}
	gboolean bSuccess = (pNewFile != NULL);
	g_object_unref (pNewFile);
	g_object_unref (pOldFile);
	return bSuccess;
}

gboolean vfs_backend_move_file (const gchar *cURI, const gchar *cDirectoryURI)
{
	g_return_val_if_fail (cURI != NULL, FALSE);
	cd_message (" %s -> %s", cURI, cDirectoryURI);
	GFile *pFile = (*cURI == '/' ? g_file_new_for_path (cURI) : g_file_new_for_uri (cURI));
	
	gchar *cFileName = g_file_get_basename (pFile);
	gchar *cNewFileURI = g_strconcat (cDirectoryURI, "/", cFileName, NULL);  // un peu moyen mais bon...
	GFile *pDestinationFile = (*cNewFileURI == '/' ? g_file_new_for_path (cNewFileURI) : g_file_new_for_uri (cNewFileURI));
	g_free (cNewFileURI);
	g_free (cFileName);
	
	GError *erreur = NULL;
	gboolean bSuccess = g_file_move (pFile,
		pDestinationFile,
		G_FILE_COPY_NOFOLLOW_SYMLINKS,
		NULL,
		NULL,  // GFileProgressCallback
		NULL,  // data
		&erreur);
	if (erreur != NULL)
	{
		cd_warning ("Attention : %s", erreur->message);
		g_error_free (erreur);
	}
	g_object_unref (pFile);
	g_object_unref (pDestinationFile);
	return bSuccess;
}

void vfs_backend_get_file_properties (const gchar *cURI, guint64 *iSize, time_t *iLastModificationTime, gchar **cMimeType, int *iUID, int *iGID, int *iPermissionsMask)
{
	g_return_if_fail (cURI != NULL);
	GFile *pFile = (*cURI == '/' ? g_file_new_for_path (cURI) : g_file_new_for_uri (cURI));
	GError *erreur = NULL;
	gchar *cQuery = g_strconcat (G_FILE_ATTRIBUTE_STANDARD_SIZE, ",", G_FILE_ATTRIBUTE_TIME_MODIFIED, ",", G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE, ",", G_FILE_ATTRIBUTE_UNIX_UID, ",", G_FILE_ATTRIBUTE_UNIX_GID, ",", G_FILE_ATTRIBUTE_ACCESS_CAN_READ, ",", G_FILE_ATTRIBUTE_ACCESS_CAN_WRITE, ",", G_FILE_ATTRIBUTE_ACCESS_CAN_EXECUTE, NULL);
	GFileInfo *pFileInfo = g_file_query_info (pFile,
		cQuery,
		G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
		NULL,
		&erreur);
	g_free (cQuery);
	if (erreur != NULL)
	{
		cd_warning ("Attention : couldn't get file properties for '%s' [%s]", cURI, erreur->message);
		g_error_free (erreur);
	}
	
	*iSize = g_file_info_get_attribute_uint64 (pFileInfo, G_FILE_ATTRIBUTE_STANDARD_SIZE);
	*iLastModificationTime = (time_t) g_file_info_get_attribute_uint64 (pFileInfo, G_FILE_ATTRIBUTE_TIME_MODIFIED);
	*cMimeType = g_file_info_get_attribute_as_string (pFileInfo, G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE);
	*iUID = g_file_info_get_attribute_uint32 (pFileInfo, G_FILE_ATTRIBUTE_UNIX_UID);
	*iGID = g_file_info_get_attribute_uint32 (pFileInfo, G_FILE_ATTRIBUTE_UNIX_GID);
	int r = g_file_info_get_attribute_uint32 (pFileInfo, G_FILE_ATTRIBUTE_ACCESS_CAN_READ);
	int w = g_file_info_get_attribute_uint32 (pFileInfo, G_FILE_ATTRIBUTE_ACCESS_CAN_WRITE);
	int x = g_file_info_get_attribute_uint32 (pFileInfo, G_FILE_ATTRIBUTE_ACCESS_CAN_EXECUTE);
	*iPermissionsMask = r * 8 * 8 + w * 8 + x;
	
	g_object_unref (pFileInfo);
	g_object_unref (pFile);
}


gchar *vfs_backend_get_trash_path (const gchar *cNearURI, gboolean bCreateIfNecessary)
{
	gchar *cPath = NULL;
	/*GFile *pFile = g_file_new_for_uri ("trash://");
	gchar *cPath = g_file_get_path (pFile);
	g_object_unref (pFile);*/
	const gchar *xdgPath = g_getenv ("XDG_DATA_HOME");
	if (xdgPath != NULL)
		cPath = g_strdup_printf ("%s/Trash/files", xdgPath);
	else
		cPath = g_strdup_printf ("%s/.local/share/Trash/files", g_getenv ("HOME"));
	return cPath;
}

gchar *vfs_backend_get_desktop_path (void)
{
	GFile *pFile = g_file_new_for_uri ("desktop://");
	gchar *cPath = g_file_get_path (pFile);
	g_object_unref (pFile);
	return cPath;
}
