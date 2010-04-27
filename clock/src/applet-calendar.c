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
#define __USE_POSIX
#include <signal.h>

#include "applet-struct.h"
#include "applet-task-editor.h"
#include "applet-calendar.h"

#define _cd_task_matches_month(pTask, iMonth, iYear) (((pTask)->iMonth == iMonth && ((pTask)->iYear == iYear || (pTask)->iFrequency == CD_TASK_EACH_YEAR)) || (pTask)->iFrequency == CD_TASK_EACH_MONTH)
#define _cd_task_matches_day(pTask, iDay, iMonth, iYear) ((pTask)->iDay == iDay && _cd_task_matches_month (pTask, iMonth, iYear))


  /////////////
 // BACKEND //
/////////////

void cd_clock_register_backend (CairoDockModuleInstance *myApplet, const gchar *cBackendName, CDClockTaskBackend *pBackend)
{
	if (myData.pBackends == NULL)
		myData.pBackends = g_hash_table_new_full (g_str_hash,
			g_str_equal,
			g_free,
			g_free);
	g_hash_table_insert (myData.pBackends, g_strdup (cBackendName), pBackend);
}

CDClockTaskBackend *cd_clock_get_backend (CairoDockModuleInstance *myApplet, const gchar *cBackendName)
{
	CDClockTaskBackend *pBackend = NULL;
	if (cBackendName != NULL)
		pBackend = g_hash_table_lookup (myData.pBackends, cBackendName);
	
	return pBackend;
}

void cd_clock_set_current_backend (CairoDockModuleInstance *myApplet)
{
	if (myData.pBackend && myData.pBackend->stop)
		myData.pBackend->stop (myApplet);
	myData.pBackend = cd_clock_get_backend (myApplet, myConfig.cTaskMgrName);
	if (myData.pBackend == NULL)
		myData.pBackend = cd_clock_get_backend (myApplet, "Default");
	if (myData.pBackend->init)
		myData.pBackend->init (myApplet);
}


  ///////////
 // TASKS //
///////////

static int _compare_task (CDClockTask *pTask1, CDClockTask *pTask2, gpointer data)
{
	if (pTask1->iYear < pTask2->iYear)
		return -1;
	if (pTask1->iYear > pTask2->iYear)
		return 1;
	
	if (pTask1->iMonth < pTask2->iMonth)
		return -1;
	if (pTask1->iMonth > pTask2->iMonth)
		return 1;
	
	if (pTask1->iDay < pTask2->iDay)
		return -1;
	if (pTask1->iDay > pTask2->iDay)
		return 1;
	
	if (pTask1->iHour < pTask2->iHour)
		return -1;
	if (pTask1->iHour > pTask2->iHour)
		return 1;
	
	if (pTask1->iMinute < pTask2->iMinute)
		return -1;
	if (pTask1->iMinute > pTask2->iMinute)
		return 1;
	
	return 0;
	
}
void cd_clock_list_tasks (CairoDockModuleInstance *myApplet)
{
	g_print ("%s ()\n", __func__);
	if (myData.pTasks != NULL)
		cd_clock_reset_tasks_list (myApplet);
	
	myData.pTasks = myData.pBackend->get_tasks (myApplet);
	CDClockTask *pTask;
	GList *t;
	for (t = myData.pTasks; t != NULL; t = t->next)
	{
		pTask = t->data;
		pTask->pApplet = myApplet;
	}
	myData.pTasks = g_list_sort_with_data (myData.pTasks,
		(GCompareDataFunc) _compare_task,
		NULL);
	myData.pNextTask = cd_clock_get_next_scheduled_task (myApplet);
}

void cd_clock_add_task_to_list (CDClockTask *pTask, CairoDockModuleInstance *myApplet)
{
	pTask->pApplet = myApplet;
	myData.pTasks = g_list_insert_sorted (myData.pTasks, pTask, (GCompareFunc)_compare_task);
	myData.pNextTask = cd_clock_get_next_scheduled_task (myApplet);
}

void cd_clock_remove_task_from_list (CDClockTask *pTask, CairoDockModuleInstance *myApplet)
{
	myData.pTasks = g_list_remove (myData.pTasks, pTask);
	myData.pNextTask = cd_clock_get_next_scheduled_task (myApplet);
}

void cd_clock_free_task (CDClockTask *pTask)
{
	if (pTask == NULL)
		return;
	if (pTask->iSidWarning != 0)
		g_source_remove (pTask->iSidWarning);
	if (pTask->pWarningDialog != NULL)
		cairo_dock_dialog_unreference (pTask->pWarningDialog);
	g_free (pTask->cTitle);
	g_free (pTask->cText);
	g_free (pTask->cTags);
	g_free (pTask->cID);
	g_free (pTask);
}

void cd_clock_reset_tasks_list (CairoDockModuleInstance *myApplet)
{
	g_list_foreach (myData.pTasks, (GFunc)cd_clock_free_task, NULL);
	g_list_free (myData.pTasks);
	myData.pTasks = NULL;
	myData.pNextTask = NULL;
}

CDClockTask *cd_clock_get_task_by_id (const gchar *cID, CairoDockModuleInstance *myApplet)
{
	if (cID == NULL)
		return NULL;
	CDClockTask *pTask;
	GList *t;
	for (t = myData.pTasks; t != NULL; t = t->next)
	{
		pTask = t->data;
		if (strcmp (pTask->cID, cID) == 0)
			return pTask;
	}
	return NULL;
}

gchar *cd_clock_get_tasks_for_today (CairoDockModuleInstance *myApplet)
{
	guint iDay = myData.currentTime.tm_mday, iMonth = myData.currentTime.tm_mon, iYear = myData.currentTime.tm_year + 1900;
	
	GString *sTaskString = NULL;
	CDClockTask *pTask;
	GList *t;
	for (t = myData.pTasks; t != NULL; t = t->next)
	{
		pTask = t->data;
		if (_cd_task_matches_day (pTask, iDay, iMonth, iYear))
		{
			if (sTaskString == NULL)
				sTaskString = g_string_new ("");
			g_string_append_printf (sTaskString, "<b><u>%s</u></b>\n <i>at %d:%02d</i>\n %s\n", pTask->cTitle ? pTask->cTitle : D_("No title"), pTask->iHour, pTask->iMinute, pTask->cText);
		}
	}
	
	if (sTaskString == NULL)
		return NULL;
	
	gchar *cTasks = sTaskString->str;
	g_string_free (sTaskString, FALSE);
	return cTasks;
}

gchar *cd_clock_get_tasks_for_this_week (CairoDockModuleInstance *myApplet)
{
	guint iDay = myData.currentTime.tm_mday, iMonth = myData.currentTime.tm_mon, iYear = myData.currentTime.tm_year + 1900;
	
	GDate* pCurrentDate = g_date_new_dmy (iDay, iMonth + 1, iYear);
	GDate* pDate = g_date_new ();
	guint d, m, y;
	int iDelta;
	GString *sTaskString = NULL;
	CDClockTask *pTask;
	GList *t;
	for (t = myData.pTasks; t != NULL; t = t->next)
	{
		pTask = t->data;
		switch (pTask->iFrequency)
		{
			case CD_TASK_DONT_REPEAT:
			default:
				d = pTask->iDay;
				m = pTask->iMonth+1;
				y = pTask->iYear;
				g_date_set_dmy (pDate, d, m, y);
				iDelta = g_date_days_between (pCurrentDate, pDate);
			break;
			
			case CD_TASK_EACH_MONTH:
				d = pTask->iDay;
				m = iMonth+1;
				y = iYear;
				g_date_set_dmy (pDate, d, m, y);
				iDelta = g_date_days_between (pCurrentDate, pDate);
				if (iDelta < 0)  // pDate est avant pCurrentDate => on teste le mois d'apres.
				{
					if (iMonth < 11)
					{
						m = iMonth+2;
						g_date_set_dmy (pDate, d, m, y);
					}
					else
					{
						m = 1;
						y = pTask->iYear + 1;
						g_date_set_dmy (pDate, d, m, y);
					}
					iDelta = g_date_days_between (pCurrentDate, pDate);
				}
			break;
			
			case CD_TASK_EACH_YEAR:
				d = pTask->iDay;
				m = pTask->iMonth+1;
				y = iYear;
				g_date_set_dmy (pDate, d, m, y);
				iDelta = g_date_days_between (pCurrentDate, pDate);
				if (iDelta < 0)  // pDate est avant pCurrentDate => on teste l'annee d'apres.
				{
					y = iYear + 1;
					g_date_set_dmy (pDate, d, m, y);
					iDelta = g_date_days_between (pCurrentDate, pDate);
				}
			break;
		}
		
		if (iDelta >= 0 && iDelta < 7)
		{
			if (sTaskString == NULL)
				sTaskString = g_string_new ("");
			g_string_append_printf (sTaskString, "<b><u>%s</u></b>\n <i>%d/%d/%d at %d:%02d</i>\n %s\n",
				pTask->cTitle ? pTask->cTitle : D_("No title"),
				(myConfig.bNormalDate ? d : y), m, (myConfig.bNormalDate ? y : d),
				pTask->iHour, pTask->iMinute,
				pTask->cText);
		}
		else if (iDelta >= 7)
			break;
	}
	g_date_free (pCurrentDate);
	
	if (sTaskString == NULL)
		return NULL;
	
	gchar *cTasks = sTaskString->str;
	g_string_free (sTaskString, FALSE);
	return cTasks;
}

#define _compute_index(y,m,d,h,mi) ((((y*12+m)*32+d)*24+h)*60+mi)
CDClockTask *cd_clock_get_next_scheduled_task (CairoDockModuleInstance *myApplet)
{
	if (myData.pTasks == NULL)
		return NULL;
	
	guint iDay = myData.currentTime.tm_mday;
	guint iMonth = myData.currentTime.tm_mon;
	guint iYear = myData.currentTime.tm_year + 1900;
	guint iHour = myData.currentTime.tm_hour;
	guint iMinute = myData.currentTime.tm_min;
	gulong iIndex = _compute_index (iYear, iMonth, iDay, iHour, iMinute);
	gulong i, iNextIndex=0;
	g_print ("%s (%d/%d/%d -> %ld)\n", __func__, iDay, iMonth, iYear, iIndex);
	
	CDClockTask *pNextTask = NULL;
	CDClockTask *pTask;
	GList *t;
	for (t = myData.pTasks; t != NULL; t = t->next)
	{
		pTask = t->data;
		g_print ("test de %s (%d/%d/%d)\n", pTask->cTitle, pTask->iDay, pTask->iMonth, pTask->iYear);
		switch (pTask->iFrequency)
		{
			case CD_TASK_DONT_REPEAT:
			default:
				i = _compute_index (pTask->iYear, pTask->iMonth, pTask->iDay, pTask->iHour, pTask->iMinute);
				g_print (" normal : %ld\n", i);
			break;
			
			case CD_TASK_EACH_MONTH:
				i = _compute_index (iYear, iMonth, pTask->iDay, pTask->iHour, pTask->iMinute);  // index pour le mois courant.
				if (i < iIndex)  // on tombe avant, on calcule l'index pour le mois suivant.
				{
					if (iMonth < 11)
						i = _compute_index (iYear, iMonth+1, pTask->iDay, pTask->iHour, pTask->iMinute);
					else
						i = _compute_index (iYear+1, 0, pTask->iDay, pTask->iHour, pTask->iMinute);
				}
				g_print (" mensuel : %ld\n", i);
			break;
			
			case CD_TASK_EACH_YEAR:
				i = _compute_index (iYear, pTask->iMonth, pTask->iDay, pTask->iHour, pTask->iMinute);
				if (i < iIndex)  // on tombe avant, on calcule l'index pour l'annee suivante.
					i = _compute_index (iYear+1, pTask->iMonth, pTask->iDay, pTask->iHour, pTask->iMinute);
				g_print (" annuel : %ld\n", i);
			break;
		}
		if (i >= iIndex && (iNextIndex == 0 || i < iNextIndex))
		{
			iNextIndex = i;
			pNextTask = pTask;
			g_print ("pNextTask <- %s, index <- %ld\n", pNextTask->cTitle, iNextIndex);
		}
	}
	return pNextTask;
}

  //////////////
 // CALENDAR //
//////////////

static void _mark_days (GtkCalendar *pCalendar, CairoDockModuleInstance *myApplet)
{
	guint iYear, iMonth, iDay;
	gtk_calendar_get_date (GTK_CALENDAR (pCalendar),
		&iYear,
		&iMonth,
		&iDay);
	
	CDClockTask *pTask;
	GList *t;
	for (t = myData.pTasks; t != NULL; t = t->next)
	{
		pTask = t->data;
		if (_cd_task_matches_month (pTask, iMonth, iYear))
		{
			gtk_calendar_mark_day (GTK_CALENDAR (pCalendar), pTask->iDay);
		}
	}
}
void cd_clock_update_calendar_marks (CairoDockModuleInstance *myApplet)
{
	if (myData.pCalendarDialog != NULL)
	{
		gtk_calendar_clear_marks (GTK_CALENDAR (myData.pCalendarDialog->pInteractiveWidget));
		_mark_days (GTK_CALENDAR (myData.pCalendarDialog->pInteractiveWidget), myApplet);
	}
}

static gchar * _on_display_task_detail (GtkCalendar *calendar, guint iYear, guint iMonth, guint iDay, CairoDockModuleInstance *myApplet)
{
	if (myData.pTasks == NULL)
		return NULL;
	
	//g_print ("%s (%d/%d/%d)\n", __func__, iDay, iMonth, iYear);
	GString *sDetail = NULL;
	CDClockTask *pTask;
	GList *t;
	for (t = myData.pTasks; t != NULL; t = t->next)
	{
		pTask = t->data;
		if (_cd_task_matches_day (pTask, iDay, iMonth, iYear))
		{
			if (sDetail == NULL)
				sDetail = g_string_new ("");
			g_string_append_printf (sDetail, "<b><u>%s</u></b>\n <i>at %d:%02d</i>\n %s\n", pTask->cTitle ? pTask->cTitle : D_("No title"), pTask->iHour, pTask->iMinute, pTask->cText);
		}
	}
	
	if (sDetail == NULL)
		return NULL;
	gchar *cDetail= sDetail->str;
	g_string_free (sDetail, FALSE);
	//g_print ("* detail : %s\n", cDetail);
	return cDetail;
}

static void _on_day_selected (GtkCalendar *pCalendar, CairoDockModuleInstance *myApplet)
{
	g_print ("%s ()\n", __func__);
	
}

static void _on_day_selected_double_click (GtkCalendar *pCalendar, CairoDockModuleInstance *myApplet)
{
	guint iDay, iMonth, iYear;
	gtk_calendar_get_date (pCalendar,
		&iYear,
		&iMonth,
		&iDay);
	cd_clock_build_task_editor (iDay, iMonth, iYear, myApplet);
}

static void _on_month_changed (GtkCalendar *pCalendar, CairoDockModuleInstance *myApplet)
{
	gtk_calendar_clear_marks (pCalendar);
	_mark_days (pCalendar, myApplet);
}

static void _on_year_changed (GtkCalendar *pCalendar, CairoDockModuleInstance *myApplet)
{
	gtk_calendar_clear_marks (pCalendar);
	_mark_days (pCalendar, myApplet);
}

static gboolean on_button_press_calendar (GtkWidget *widget,
	GdkEventButton *pButton,
	CairoDockModuleInstance *myApplet)
{
	myData.iButtonPressTime = pButton->time;
	return FALSE;
}

static GtkWidget *cd_clock_build_calendar (CairoDockModuleInstance *myApplet)
{
	cd_message ("%s ()", __func__);
	GtkWidget *pCalendar = gtk_calendar_new ();
	g_object_set (G_OBJECT (pCalendar), "show-details", FALSE, NULL);
	
	_mark_days (GTK_CALENDAR (pCalendar), myApplet);
	
	g_signal_connect (G_OBJECT (pCalendar), "day-selected" , G_CALLBACK (_on_day_selected), myApplet);
	g_signal_connect (G_OBJECT (pCalendar), "day-selected-double-click" , G_CALLBACK (_on_day_selected_double_click), myApplet);
	g_signal_connect (G_OBJECT (pCalendar), "prev-month" , G_CALLBACK (_on_month_changed), myApplet);
	g_signal_connect (G_OBJECT (pCalendar), "next-month" , G_CALLBACK (_on_month_changed), myApplet);
	g_signal_connect (G_OBJECT (pCalendar), "prev-year" , G_CALLBACK (_on_year_changed), myApplet);
	g_signal_connect (G_OBJECT (pCalendar), "next-year" , G_CALLBACK (_on_year_changed), myApplet);
	g_signal_connect (G_OBJECT (pCalendar),
		"button-press-event",
		G_CALLBACK (on_button_press_calendar),
		myApplet);
	
#if (GTK_MAJOR_VERSION > 2 || GTK_MINOR_VERSION > 14)
	gtk_calendar_set_detail_func (GTK_CALENDAR (pCalendar),
		(GtkCalendarDetailFunc) _on_display_task_detail,
		myApplet,
		(GDestroyNotify) NULL);
#endif
	return pCalendar;
}

void cd_clock_hide_dialogs (CairoDockModuleInstance *myApplet)
{
	cairo_dock_remove_dialog_if_any (myIcon);
	myData.pCalendarDialog = NULL;
}

static gboolean on_button_press_dialog (GtkWidget *widget,
	GdkEventButton *pButton,
	CairoDockModuleInstance *myApplet)
{
	CD_APPLET_ENTER;
	if (pButton->time > myData.iButtonPressTime)
	{
		cairo_dock_dialog_unreference (myData.pCalendarDialog);
		myData.pCalendarDialog = NULL;
	}
	CD_APPLET_LEAVE (FALSE);
}
void cd_clock_show_hide_calendar (CairoDockModuleInstance *myApplet)
{
	if (myData.pCalendarDialog != NULL)
	{
		cairo_dock_dialog_unreference (myData.pCalendarDialog);
		myData.pCalendarDialog = NULL;
		if (myData.pTaskWindow != NULL)
		{
			gtk_widget_destroy (myData.pTaskWindow);
			myData.pTaskWindow = NULL;
			myData.pModel = NULL;
		}
	}
	else
	{
		cairo_dock_remove_dialog_if_any (myIcon);
		GtkWidget *pCalendar = cd_clock_build_calendar (myApplet);
		myData.pCalendarDialog = cairo_dock_show_dialog_full (D_("Calendar"),
			myIcon, myContainer,
			0,
			MY_APPLET_SHARE_DATA_DIR"/dates.svg",
			pCalendar,
			NULL, NULL, NULL);
		g_signal_connect (G_OBJECT (myData.pCalendarDialog->container.pWidget),
			"button-press-event",
			G_CALLBACK (on_button_press_dialog),
			myApplet);
	}
}