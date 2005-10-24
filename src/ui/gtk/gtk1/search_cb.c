/*
 * $Id$
 *
 * Copyright (c) 2001-2003, Raphael Manfredi, Richard Eckart
 *
 *----------------------------------------------------------------------
 * This file is part of gtk-gnutella.
 *
 *  gtk-gnutella is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  gtk-gnutella is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with gtk-gnutella; if not, write to the Free Software
 *  Foundation, Inc.:
 *      59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *----------------------------------------------------------------------
 */

/**
 * @ingroup gtk
 * @file
 *
 * GUI filtering functions.
 *
 * @author Raphael Manfredi
 * @author Richard Eckart
 * @date 2001-2003
 */

#include "gtk/gui.h"

RCSID("$Id$");

#include <gdk/gdkkeysyms.h>

#include "gtk/gtkcolumnchooser.h"
#include "gtk/search.h"
#include "gtk/statusbar.h"
#include "gtk/gtk-missing.h"
#include "gtk/bitzi.h"			/* Bitzi GTK functions */
#include "gtk/columns.h"
#include "gtk/misc.h"
#include "search_cb.h"

#include "if/gui_property.h"
#include "if/gnet_property.h"
#include "if/bridge/ui2c.h"

#include "lib/glib-missing.h"
#include "lib/iso3166.h"
#include "lib/vendors.h"
#include "lib/utf8.h"
#include "lib/override.h"		/* Must be the last header included */

extern search_t *search_selected;

/***
 *** Private functions
 ***/


/**
 *	Activates/deactivates buttons and popups based on what is selected
 */
static void
refresh_popup(void)
{
	/*
	 * The following popup items are set insensitive if nothing is currently
	 * selected.
	 */
	static const struct {
		const gchar *name;
	} menu[] = {
		{	"popup_search_drop" },
		{	"popup_search_drop_global" },
		{	"popup_search_autodownload" },
		{	"popup_search_new_from_selected" },
		{	"popup_search_metadata" },
		{	"popup_search_browse_host" },
	};
	search_t *search = search_gui_get_current_search();
	gboolean sensitive;
	guint i;

	sensitive = NULL != search && NULL != GTK_CLIST(search->ctree)->selection;
	gtk_widget_set_sensitive(
			lookup_widget(main_window, "button_search_download"),
			sensitive);

	for (i = 0; i < G_N_ELEMENTS(menu); i++)
		gtk_widget_set_sensitive(lookup_widget(popup_search, menu[i].name),
			sensitive);

	sensitive = NULL != search;	
    gtk_widget_set_sensitive(
        lookup_widget(popup_search, "popup_search_restart"), sensitive);
    gtk_widget_set_sensitive(
        lookup_widget(popup_search, "popup_search_duplicate"), sensitive);

    if (search) {
        gtk_widget_set_sensitive(
            lookup_widget(popup_search, "popup_search_stop"),
			!guc_search_is_frozen(search->search_handle));
		gtk_widget_set_sensitive(
            lookup_widget(popup_search, "popup_search_resume"),
			guc_search_is_frozen(search->search_handle));
		if (search->passive)
            gtk_widget_set_sensitive(
                lookup_widget(popup_search, "popup_search_restart"),
                FALSE);
    } else {
        gtk_widget_set_sensitive(
            lookup_widget(popup_search, "popup_search_stop"), FALSE);
	    gtk_widget_set_sensitive(
            lookup_widget(popup_search, "popup_search_resume"), FALSE);
    }
}

/**
 * Set or clear (when rc == NULL) the information about the search.
 */
static void
search_gui_set_details(const record_t *rc)
{
	enum info_idx {
		info_filename = 0,
		info_sha1,
		info_source,
		info_size,
		info_guid,
		info_timestamp,
		info_vendor,
		info_index,
		info_tag,
		info_country,
		info_speed,

		num_infos
	};
	static GtkEntry *entry[num_infos];
	static GtkText *xml;
	static gboolean initialized;
	gchar *xml_text;
	guint j;

	if (!initialized) {
		static const struct {
			const gchar *name;
		} widgets[] = {
#define D(x) STRINGIFY(CAT2(entry_result_,x))
			{ D(info_filename) },
			{ D(info_sha1) },
			{ D(info_source) },
			{ D(info_size) },
			{ D(info_guid) },
			{ D(info_timestamp) },
			{ D(info_vendor) },
			{ D(info_index) },
			{ D(info_tag) },
			{ D(info_country) },
			{ D(info_speed) },
#undef D
		};
		guint i;
		
		STATIC_ASSERT(G_N_ELEMENTS(widgets) == G_N_ELEMENTS(entry));
		STATIC_ASSERT(G_N_ELEMENTS(widgets) == num_infos);
		
		initialized = TRUE;

		for (i = 0; i < G_N_ELEMENTS(entry); i++)
			entry[i] = GTK_ENTRY(lookup_widget(main_window, widgets[i].name));

		xml = GTK_TEXT(lookup_widget(main_window,
							STRINGIFY(CAT2(text_result_,info_xml))));
	}

	if (NULL == rc) {
		guint i;

		for (i = 0; i < G_N_ELEMENTS(entry); i++)
			gtk_entry_set_text(entry[i], "");

		gtk_text_freeze(xml);
		gtk_text_set_point(xml, 0);
		gtk_text_forward_delete(xml, gtk_text_get_length(xml));
		gtk_text_thaw(xml);

		return;
	}

	for (j = 0; j < num_infos; j++) {
		GtkEntry *e = entry[j];

		switch ((enum info_idx) j) {
		case info_filename:
			gtk_entry_set_text(e, rc->name);
			break;
			
		case info_sha1:
			gtk_entry_printf(e, "%s%s",
				rc->sha1 ? "urn:sha1:" : _("<no SHA1 known>"),
				rc->sha1 ? sha1_base32(rc->sha1) : "");
			break;
			
		case info_source:
			gtk_entry_set_text(e,
				rc->results_set->hostname
					? hostname_port_to_gchar(rc->results_set->hostname,
						rc->results_set->port)
					: host_addr_port_to_string(rc->results_set->addr,
						rc->results_set->port));
			break;

		case info_country:
			gtk_entry_printf(e, "%s (%s)",
				iso3166_country_name(rc->results_set->country),
				iso3166_country_cc(rc->results_set->country));
			break;

		case info_size:
			{
				gchar bytes[32];
				
				uint64_to_string_buf(rc->size, bytes, sizeof bytes);
				gtk_entry_printf(e, _("%s (%s bytes)"),
					short_size(rc->size), bytes);
			}
			break;

		case info_guid:
			gtk_entry_set_text(e, guid_hex_str(rc->results_set->guid));
			break;

		case info_timestamp:
			/* The ".24" is there to discard the trailing '\n' (see ctime(3) */
			gtk_entry_printf(entry[info_timestamp], "%24.24s",
				ctime(&rc->results_set->stamp));
			break;

		case info_vendor:
			{	
				const gchar *vendor, *ver;

				vendor = lookup_vendor_name(rc->results_set->vendor);
				ver = vendor ? rc->results_set->version : NULL;
				gtk_entry_printf(e, "%s%s%s",
					EMPTY_STRING(vendor),
					ver ? "/" : "",
					EMPTY_STRING(ver));
			}
			break;

		case info_index:
			gtk_entry_printf(e, "%lu", (gulong) rc->index);
			break;

		case info_tag:
			gtk_entry_set_text(e, EMPTY_STRING(rc->tag));
			break;
			
		case info_speed:
			gtk_entry_printf(e, "%u", rc->results_set->speed);
			break;

		case num_infos:
			g_assert_not_reached();
		}
	}

	xml_text = rc->xml ? search_xml_indent(rc->xml) : NULL;
	if (NULL != xml_text) {
		gtk_text_freeze(xml);
		gtk_text_set_point(xml, 0);
		gtk_text_insert(xml, NULL, NULL, NULL,
			lazy_utf8_to_locale(xml_text), -1);
		gtk_text_thaw(xml);
	}
	G_FREE_NULL(xml_text);
}

/**
 *	Autoselects all searches matching given node in given tree
 */
gint
search_cb_autoselect(GtkCTree *ctree, GtkCTreeNode *node)
{
	GtkCTreeNode *auto_node;
	GtkCTreeNode *parent, *child;
	gui_record_t *grc;
	gui_record_t *grc2;
	record_t *rc;
	record_t *rc2;
	guint32 sel_files = 0;
    guint32 sel_sources = 0;
    gboolean frozen = FALSE;

	/*
     * Rows with NULL data can appear when inserting new rows
     * because the selection is resynced and the row data cannot
     * be set until insertion and therefore also selection syncing
     * is done.
     *      -- Richard, 20/06/2002
     *
     * Can this really happen???
     *      -- Richard, 18/04/2004
     */
	grc = gtk_ctree_node_get_row_data(ctree, node);
    if (grc == NULL) {
        g_warning("search_cb_autoselect: row with NULL data detected");
        return 0;
    }

	rc = grc->shared_record;

	/*
	 * Update details about the selected search.
	 */
	search_gui_set_details(rc);

    /* Search the top level of the tree for items to auto-select.
     * We don't want to search the children, because on the
     * relevant data, all the children are the same as thier parents
     * anyway (stored in the shared_record) */
	for (
        auto_node = GTK_CTREE_NODE(GTK_CLIST(ctree)->row_list);
		(NULL != auto_node) && (NULL != rc);
		auto_node = GTK_CTREE_NODE_SIBLING (auto_node)
    ) {
		if (NULL == auto_node)
			continue;

		grc2 = gtk_ctree_node_get_row_data(ctree, auto_node);
		rc2 = grc2->shared_record;

        if (rc2 == NULL) {
        	g_warning("on_ctree_search_results_select_row: "
                "detected row with NULL data, skipping.");
            continue;
		}

		/* XXX anti-crash, when rc is 0x3 or something... -- RAM, 16/03/2004 */
		if ((gulong) rc2 < 0x1000) {
			statusbar_gui_message(15,
				"*** MEMORY CORRUPTED, TRYING TO IGNORE!");
            g_warning("MEMORY CORRUPTED (in GUI search row data, rc=0x%lx)",
                (gulong) rc2);
			continue;
		}

        /* Check if the current node matches. If this is true, then
         * we also select all the children without checking further
         * if they match too. All the children share the same "shared_record"
         * with thier parents and that contains the only relevant data
         * on which the matching is done.
         *     -- Richard, 18/04/2004
         */
        if (
            (NULL != rc) && (NULL != rc2) &&
			((rc == rc2) || ((rc->sha1 != NULL) && (rc->sha1 == rc2->sha1)))
        ) {
			gtk_ctree_select(ctree, auto_node);
            sel_files ++;
            sel_sources ++;

            parent = auto_node;
            child  = GTK_CTREE_ROW(auto_node)->children;

            for (; NULL != child; child = GTK_CTREE_NODE_SIBLING(child)) {
                if (!frozen && gtk_ctree_is_viewable(ctree, child)) {
                    gtk_clist_freeze(GTK_CLIST(ctree));
                    frozen = TRUE;
                }
                gtk_ctree_select(ctree, child);
				sel_sources ++;
            }
		}
	} /* top-level node iteration loop (for)*/

    if (frozen) {
        gtk_clist_thaw(GTK_CLIST(ctree));
    }

    if (sel_sources > 1) {
        statusbar_gui_message(15,
            NG_("%d file auto selected with %d sources %s",
                "%d files auto selected with %d sources %s", sel_files),
            sel_files, sel_sources, _("by urn:sha1."));
    }

    /* The quest for reduced gui flickering...
     * Gtk1 seems to have an odd system of determining when to
     * redraw what. These conditions seem to reduce need to
     * redraw quite well while still guaranteeing that Gtk1
     * redraws the gui when necessary:
     * - If more than one top-level line is affected (sel_files)
     * - If a visible child was affected (frozen).
     *     -- Richard, 18/04/2004
     */
    if (frozen || (sel_files > 1)) {
        gtk_widget_queue_draw((GtkWidget *) ctree); /* Force redraw */
    }

	return (sel_files+sel_sources);
}




/***
 *** Glade callbacks
 ***/
void
on_combo_entry_searches_activate(GtkEditable *unused_editable,
	gpointer unused_udata)
{
    /* FIXME */
	(void) unused_editable;
	(void) unused_udata;
}


/**
 */
void
on_search_popdown_switch(GtkWidget *unused_w, gpointer unused_data)
{
	(void) unused_w;
	(void) unused_data;

	if (search_selected != NULL)
        search_gui_set_current_search(search_selected);
}


/**
 *	When the user switches notebook tabs, update the rest of GUI
 *
 *	This may be obsolete as we removed the tabbed interface --Emile 27/12/03
 */
void
on_search_notebook_switch(GtkNotebook *notebook, GtkNotebookPage *unused_page,
	gint page_num, gpointer unused_udata)
{
	search_t *sch;

	(void) unused_page;
	(void) unused_udata;

	sch = (search_t *) gtk_object_get_user_data(
		GTK_OBJECT(gtk_notebook_get_nth_page(notebook, page_num)));

	g_return_if_fail(sch);

    search_gui_set_current_search(sch);
}


/**
 *	Changes current search and updates GUI
 */
void
on_clist_search_select_row(GtkCList *clist, gint row, gint unused_column,
	GdkEvent *unused_event, gpointer unused_udata)
{
    search_t *sch;

	(void) unused_column;
	(void) unused_event;
	(void) unused_udata;
    g_assert(clist != NULL);

    sch = gtk_clist_get_row_data(clist, row);
    if (sch == NULL)
        return;

    search_gui_set_current_search(sch);
}


/**
 */
void
on_search_selected(GtkItem *unused_item, gpointer data)
{
	(void) unused_item;
	search_selected = (search_t *) data;
}


/**
 *	Create a search based on query entered
 */
void
on_button_search_clicked(GtkButton *unused_button, gpointer unused_udata)
{
	GtkWidget *widget;
	gchar *e;

	/*
	 * Even though we might not be on_the_net() yet, record the search.
	 * There is a callback mechanism when a new node is connected, which
	 * will launch the search there if it has not been sent already.
	 *		--patch from Mark Schreiber, 10/01/2002
	 */

	(void) unused_button;
	(void) unused_udata;

    widget = lookup_widget(main_window, "entry_search");
   	e = STRTRACK(gtk_editable_get_chars(GTK_EDITABLE(widget), 0, -1));
    g_strstrip(e);
    if (*e) {
        filter_t *default_filter;
        search_t *search;
        gboolean res;

        /*
         * It's important gui_search_history_add is called before
         * new_search, otherwise the search entry will not be
         * cleared.
         *      --BLUE, 04/05/2002
         */
        gui_search_history_add(e);

        /*
         * We have to capture the selection here already, because
         * new_search will trigger a rebuild of the menu as a
         * side effect.
         */
        default_filter = (filter_t *)option_menu_get_selected_data
            (lookup_widget(main_window, "optionmenu_search_filter"));

		res = search_gui_new_search(e, 0, &search);

        /*
         * If we should set a default filter, we do that.
         */
        if (res && (default_filter != NULL)) {
            rule_t *rule;
		   
			rule = filter_new_jump_rule(default_filter, RULE_FLAG_ACTIVE);

            /*
             * Since we don't want to distrub the shadows and
             * do a "force commit" without the user having pressed
             * the "ok" button in the dialog, we add the rule
             * manually.
             */
            search->filter->ruleset =
                g_list_append(search->filter->ruleset, rule);
            rule->target->refcount ++;
        }

        if (!res)
        	gdk_beep();
    }

	gtk_widget_grab_focus(widget);
	G_FREE_NULL(e);
}


/**
 */
void
on_entry_search_activate(GtkEditable *unused_editable, gpointer user_data)
{
    /*
     * Delegate to: on_button_search_clicked.
     *      --BLUE, 30/04/2002
     */

	(void) unused_editable;

	on_button_search_clicked(NULL, user_data);
}


/**
 *	When a search string is entered, activate the search button
 */
void
on_entry_search_changed(GtkEditable *editable, gpointer unused_udata)
{
	gchar *e = STRTRACK(gtk_editable_get_chars(editable, 0, -1));

	(void) unused_udata;
	g_strstrip(e);
	gtk_widget_set_sensitive
        (lookup_widget(main_window, "button_search"), *e != 0);
	G_FREE_NULL(e);
}


/**
 *	Clear search results, de-activate clear search button
 */
void
on_button_search_clear_clicked(GtkButton *unused_button, gpointer unused_udata)
{
	(void) unused_button;
	(void) unused_udata;

	gui_search_clear_results();
	gtk_widget_set_sensitive
        (lookup_widget(main_window, "button_search_clear"), FALSE);
}


/**
 */
void
on_button_search_close_clicked(GtkButton *unused_button, gpointer unused_udata)
{
    search_t *search;

	(void) unused_button;
	(void) unused_udata;

    search = search_gui_get_current_search();
    if (search != NULL)
        search_gui_close_search(search);
}


/**
 */
void
on_button_search_download_clicked(GtkButton *unused_button,
	gpointer unused_udata)
{
	(void) unused_button;
	(void) unused_udata;

    search_gui_download_files();
}


/**
 */
void
on_button_search_collapse_all_clicked(GtkButton *unused_button,
	gpointer unused_udata)
{
	(void) unused_button;
	(void) unused_udata;

    search_gui_collapse_all();
}


/**
 */
void
on_button_search_expand_all_clicked(GtkButton *unused_button,
	gpointer unused_udata)
{
	(void) unused_button;
	(void) unused_udata;

    search_gui_expand_all();
}


/**
 */
gboolean
on_clist_search_results_key_press_event(GtkWidget *unused_widget,
	GdkEventKey *event, gpointer unused_udata)
{
    g_assert(event != NULL);

	(void) unused_widget;
	(void) unused_udata;

    switch (event->keyval) {
    case GDK_Return:
        search_gui_download_files();
        return TRUE;
	case GDK_Delete:
        search_gui_discard_files();
		return TRUE;
    default:
        return FALSE;
    }
}


/**
 *	Handles showing the popup in the event of right-clicks and downloading
 *	for double-clicks
 */
gboolean
on_clist_search_results_button_press_event(GtkWidget *widget,
	GdkEventButton *event, gpointer unused_udata)
{
	(void) unused_udata;

	switch (event->button) {
	case 1:
        /* left click section */
		if (event->type == GDK_2BUTTON_PRESS) {
			gtk_signal_emit_stop_by_name(GTK_OBJECT(widget),
				"button_press_event");
			return FALSE;
		}
		if (event->type == GDK_BUTTON_PRESS) {
			static guint click_time = 0;

			if ((event->time - click_time) <= 250) {
				gint row = 0;
				gint column = 0;

				/*
				 * 2 clicks within 250 msec == doubleclick.
				 * Surpress further events
				 */
				gtk_signal_emit_stop_by_name(GTK_OBJECT(widget),
					"button_press_event");

				if (gtk_clist_get_selection_info(GTK_CLIST(widget), event->x,
					event->y, &row, &column)) {

					search_gui_download_files();
                    return TRUE;
				}
			} else {
				click_time = event->time;
				return FALSE;
			}
		}
		return FALSE;

	case 3:
        /* right click section (popup menu) */
    	if (search_gui_get_current_search()) {
			GtkMenuItem *item;
            gboolean search_results_show_tabs;

        	refresh_popup();
            gui_prop_get_boolean_val(PROP_SEARCH_RESULTS_SHOW_TABS,
                &search_results_show_tabs);

			item = GTK_MENU_ITEM(lookup_widget(popup_search,
									"popup_search_toggle_tabs"));
        	gtk_label_set(GTK_LABEL(item->item.bin.child),
				search_results_show_tabs
					? _("Show search list")
					: _("Show tabs"));
			gtk_menu_popup(GTK_MENU(popup_search), NULL, NULL, NULL, NULL,
                     event->button, event->time);
        }
		return TRUE;
	}

	return FALSE;
}


/**
 */
void
on_button_search_filter_clicked(GtkButton *unused_button, gpointer unused_udata)
{
	(void) unused_button;
	(void) unused_udata;

	filter_open_dialog();
}


/**
 *	Sort search according to selected column
 */
void
on_clist_search_results_click_column(GtkCList *clist, gint column,
	gpointer unused_udata)
{
    search_t *search;

	(void) unused_udata;
    g_assert(clist != NULL);

    search = search_gui_get_current_search();
	if (search == NULL)
		return;

    /* rotate or initialize search order */
	if (column == search->sort_col) {
        switch (search->sort_order) {
        case SORT_ASC:
            search->sort_order = SORT_DESC;
           	break;
        case SORT_DESC:
            search->sort_order = SORT_NONE;
            break;
        case SORT_NONE:
            search->sort_order = SORT_ASC;
        }
	} else {
		search->sort_col = column;
		search->sort_order = SORT_ASC;
	}

	search_gui_sort_column(search, column); /* Sort column, draw arrow */
}


/**
 *	This function is called when the user selects a row in the
 *	search results pane. Autoselection takes place here.
 */
void
on_ctree_search_results_select_row(GtkCTree *ctree,
	GList *node, gint unused_column, gpointer unused_udata)
{
    static gboolean active = FALSE;

	(void) unused_column;
	(void) unused_udata;

    /*
     * We need to avoid recursion to prevent memory corruption.
     */
	if (active)
		return;

	if (NULL == node)
		return;

    active = TRUE;

    /* actually using the "active" guard should be enough, but
     * I'll try to block the signal too, just to be on the save side.
     */
    /* FIXME: Use either guard or signal blocking. Signal blocking
     *        should be preferred to get better performance.
	 */
    gtk_signal_handler_block_by_func(GTK_OBJECT(ctree),
		GTK_SIGNAL_FUNC(on_ctree_search_results_select_row),
		NULL);

    refresh_popup();
	search_cb_autoselect(ctree, GTK_CTREE_NODE(node));

    gtk_signal_handler_unblock_by_func(GTK_OBJECT(ctree),
        GTK_SIGNAL_FUNC(on_ctree_search_results_select_row),
        NULL);

    active = FALSE;
}


/**
 */
void
on_ctree_search_results_unselect_row(GtkCTree *unused_ctree, GList *unused_node,
	gint unused_column, gpointer unused_udata)
{
	(void) unused_ctree;
	(void) unused_node;
	(void) unused_column;
	(void) unused_udata;

    refresh_popup();
	search_gui_set_details(NULL);	/* Clear details about search */
}


/**
 */
void
on_ctree_search_results_resize_column(GtkCList *unused_clist, gint column,
	gint width, gpointer unused_udata)
{
    guint32 buf = width;

	(void) unused_clist;
	(void) unused_udata;

    /* remember the width for storing it to the config file later */
    gui_prop_set_guint32(PROP_SEARCH_RESULTS_COL_WIDTHS, &buf, column, 1);
}


/**
 *	Please add comment
 */
void
on_button_search_passive_clicked(GtkButton *unused_button,
	gpointer unused_udata)
{
    filter_t *default_filter;
	search_t *search;

	(void) unused_button;
	(void) unused_udata;

    /*
     * We have to capture the selection here already, because
     * new_search will trigger a rebuild of the menu as a
     * side effect.
     */
    default_filter = (filter_t *)
        option_menu_get_selected_data
            (lookup_widget(main_window, "optionmenu_search_filter"));

	search_gui_new_search(_("Passive"), SEARCH_PASSIVE, &search);

    /*
     * If we should set a default filter, we do that.
     */
    if (default_filter != NULL) {
        rule_t *rule = filter_new_jump_rule(default_filter, RULE_FLAG_ACTIVE);

        /*
         * Since we don't want to distrub the shadows and
         * do a "force commit" without the user having pressed
         * the "ok" button in the dialog, we add the rule
         * manually.
         */
        search->filter->ruleset =
            g_list_append(search->filter->ruleset, rule);
        rule->target->refcount ++;
    }
}



/***
 *** Search results popup
 ***/


/**
 *	Given a GList of GtkCTreeNodes, return a new list pointing to the shared
 *	record contained by the row data.
 *	List will have to be freed later on.
 */
GSList *
search_cb_collect_ctree_data(GtkCTree *ctree,
	GList *node_list, GCompareFunc cfn)
{
	GSList *data_list = NULL;
	gui_record_t *grc;
	record_t *rc;

	for (/* empty */; node_list != NULL; node_list = g_list_next(node_list)) {

		if (node_list->data != NULL) {
			grc = gtk_ctree_node_get_row_data(ctree, node_list->data);
			rc = grc->shared_record;
			if (!cfn || NULL == g_slist_find_custom(data_list, rc, cfn))
				data_list = g_slist_prepend(data_list, rc);
		}
	}

	return g_slist_reverse(data_list);
}

static void
add_filter(filter_t *filter, GFunc filter_add_func, GCompareFunc cfn)
{
    GList *node_list;
    GSList *data_list = NULL;
    search_t *search;

    search = search_gui_get_current_search();
    g_assert(search != NULL);

    gtk_clist_freeze(GTK_CLIST(search->ctree));

	node_list = g_list_copy(GTK_CLIST(search->ctree)->selection);
	data_list = search_cb_collect_ctree_data(search->ctree, node_list, cfn);

    g_slist_foreach(data_list, filter_add_func, filter);

    gtk_clist_thaw(GTK_CLIST(search->ctree));
	g_slist_free(data_list);
	g_list_free(node_list);
}

static void
search_add_filter(GFunc filter_add_func, GCompareFunc cfn)
{
    search_t *search;
	
    search = search_gui_get_current_search();
    g_assert(search != NULL);
	
	add_filter(search->filter, filter_add_func, cfn);
}

static void
global_add_filter(GFunc filter_add_func, GCompareFunc cfn)
{
	add_filter(filter_get_global_pre(), filter_add_func, cfn);
}

/**
 *	For all selected results, create a filter based on name
 */
void
on_popup_search_drop_name_activate(GtkMenuItem *unused_menuitem,
	gpointer unused_udata)
{
	(void) unused_menuitem;
	(void) unused_udata;

    search_add_filter((GFunc) filter_add_drop_name_rule, gui_record_name_eq);
}


/**
 *	For all selected results, create a filter based on sha1
 */
void
on_popup_search_drop_sha1_activate(GtkMenuItem *unused_menuitem,
	gpointer unused_udata)
{
	(void) unused_menuitem;
	(void) unused_udata;
	
    search_add_filter((GFunc) filter_add_drop_sha1_rule, gui_record_sha1_eq);
}


/**
 *	For all selected results, create a filter based on host
 */
void
on_popup_search_drop_host_activate(GtkMenuItem *unused_menuitem,
	gpointer unused_udata)
{
	(void) unused_menuitem;
	(void) unused_udata;

    search_add_filter((GFunc) filter_add_drop_host_rule, gui_record_host_eq);
}


/**
 *	For all selected results, create a global filter based on name
 */
void
on_popup_search_drop_name_global_activate(GtkMenuItem *unused_menuitem,
	gpointer unused_udata)
{
	(void) unused_menuitem;
	(void) unused_udata;

    global_add_filter((GFunc) filter_add_drop_name_rule, gui_record_name_eq);
}

/**
 *	For all selected results, create a global filter based on sha1
 */
void
on_popup_search_drop_sha1_global_activate(GtkMenuItem *unused_menuitem,
	gpointer unused_udata)
{
	(void) unused_menuitem;
	(void) unused_udata;

    global_add_filter((GFunc) filter_add_drop_sha1_rule, gui_record_sha1_eq);
}

/**
 *	For all selected results, create a global filter based on host
 */
void on_popup_search_drop_host_global_activate(GtkMenuItem *unused_menuitem,
	gpointer unused_udata)
{
	(void) unused_menuitem;
	(void) unused_udata;

    global_add_filter((GFunc) filter_add_drop_host_rule, gui_record_host_eq);
}


/**
 *	Please add comment
 */
void
on_popup_search_autodownload_name_activate(GtkMenuItem *unused_menuitem,
	gpointer unused_udata)
{
	(void) unused_menuitem;
	(void) unused_udata;

    search_add_filter((GFunc) filter_add_download_name_rule,
		gui_record_name_eq);
}


/**
 *	Please add comment
 */
void
on_popup_search_autodownload_sha1_activate(GtkMenuItem *unused_menuitem,
	gpointer unused_udata)
{
	(void) unused_menuitem;
	(void) unused_udata;

    search_add_filter((GFunc) filter_add_download_sha1_rule,
		gui_record_sha1_eq);
}


/**
 *	Please add comment
 */
void
on_popup_search_new_from_selected_activate(GtkMenuItem *unused_menuitem,
	gpointer unused_udata)
{
    GList *node_list;
	GSList *data_list;
    search_t *search;

	(void) unused_menuitem;
	(void) unused_udata;

    search = search_gui_get_current_search();
    g_assert(search != NULL);

    gtk_clist_freeze(GTK_CLIST(search->ctree));

	node_list = g_list_copy(GTK_CLIST(search->ctree)->selection);
	data_list = search_cb_collect_ctree_data(search->ctree,
					node_list, gui_record_name_eq);

    g_slist_foreach(data_list, (GFunc) search_gui_add_targetted_search, NULL);

	gtk_clist_thaw(GTK_CLIST(search->ctree));
	g_slist_free(data_list);
	g_list_free(node_list);
}


/**
 *	Please add comment
 */
void
on_popup_search_edit_filter_activate(GtkMenuItem *unused_menuitem,
	gpointer unused_udata)
{
	(void) unused_menuitem;
	(void) unused_udata;

    filter_open_dialog();
}


/**
 *	Create a new search identical to the current search.
 *
 * 	@note Doesn't duplicate filters or passive searches yet.
 */
void
on_popup_search_duplicate_activate(GtkMenuItem *unused_menuitem,
	gpointer unused_udata)
{
    search_t *search;
    guint32 timeout;

	(void) unused_menuitem;
	(void) unused_udata;

    gnet_prop_get_guint32_val
		(PROP_SEARCH_REISSUE_TIMEOUT, &timeout);

    search = search_gui_get_current_search();
    /* FIXME: should also duplicate filters! */
    /* FIXME: should call search_duplicate which has to be written. */
    /* FIXME: should properly duplicate passive searches. */
	if (search)
		search_gui_new_search_full(search->query,
			timeout, search->sort_col, search->sort_order,
			search->enabled ? SEARCH_ENABLED : 0, NULL);
}


/**
 *	Please add comment
 */
void
on_popup_search_restart_activate(GtkMenuItem *unused_menuitem,
	gpointer unused_udata)
{
    search_t *search;

	(void) unused_menuitem;
	(void) unused_udata;

    search = search_gui_get_current_search();
	if (search)
		search_gui_restart_search(search);
}

/**
 *	Please add comment
 */
void
on_popup_search_resume_activate(GtkMenuItem *unused_menuitem,
	gpointer unused_udata)
{
    search_t *search;

	(void) unused_menuitem;
	(void) unused_udata;

    search = search_gui_get_current_search();
	if (search)
		gui_search_set_enabled(search, TRUE);
}

/**
 *	Stop current search
 */
void
on_popup_search_stop_activate(GtkMenuItem *unused_menuitem,
	gpointer unused_udata)
{
    search_t *search;

	(void) unused_menuitem;
	(void) unused_udata;

    search = search_gui_get_current_search();
	if (search)
		gui_search_set_enabled(search, FALSE);
}


/**
 *	Please add comment
 */
void
on_popup_search_config_cols_activate(GtkMenuItem *unused_menuitem,
	gpointer unused_udata)
{
    GtkWidget * cc;
    search_t *search;

	(void) unused_menuitem;
	(void) unused_udata;

    search = search_gui_get_current_search();
    g_return_if_fail(search != NULL);
    g_assert(search->ctree != NULL);

    cc = gtk_column_chooser_new(GTK_WIDGET(search->ctree));
    gtk_menu_popup(GTK_MENU(cc), NULL, NULL, NULL, NULL, 1, 0);

    /* GtkColumnChooser takes care of cleaning up itself */
}

/**
 *	Please add comment
 */
void
on_popup_search_expand_all_activate(GtkMenuItem *unused_menuitem,
	gpointer unused_udata)
{
	(void) unused_menuitem;
	(void) unused_udata;

    search_gui_expand_all();

}

/**
 *	Please add comment
 */
void
on_popup_search_collapse_all_activate(GtkMenuItem *unused_menuitem,
	gpointer unused_udata)
{
	(void) unused_menuitem;
	(void) unused_udata;

    search_gui_collapse_all();

}

/**
 * Queue a bitzi queries from the search context menu
 */
void
on_popup_search_metadata_activate(GtkMenuItem *unused_menuitem,
	gpointer unused_udata)
{
    GList *node_list;
	GSList *data_list;
    search_t *search;
	guint32 bitzi_debug;

	(void) unused_menuitem;
	(void) unused_udata;

    search = search_gui_get_current_search();
    g_assert(search != NULL);

    gtk_clist_freeze(GTK_CLIST(search->ctree));

	node_list = g_list_copy(GTK_CLIST(search->ctree)->selection);
	data_list = search_cb_collect_ctree_data(search->ctree,
					node_list, gui_record_sha1_eq);

	/* Make sure the column is actually visible. */
	{
		static const gint min_width = 80;
		GtkCList *clist = GTK_CLIST(search->ctree);

    	gtk_clist_set_column_visibility(clist, c_sr_meta, TRUE);
		if (clist->column[c_sr_meta].width < min_width)
    		gtk_clist_set_column_width(clist, c_sr_meta, min_width);
	}
	
	/* Queue up our requests */
    gnet_prop_get_guint32_val(PROP_BITZI_DEBUG, &bitzi_debug);
	if (bitzi_debug)
		g_message("on_popup_search_metadata_activate: %d items, %p",
			  g_slist_position(data_list, g_slist_last(data_list)) + 1,
			  cast_to_gconstpointer(data_list));

	G_SLIST_FOREACH(data_list, search_gui_queue_bitzi_by_sha1, NULL);

	gtk_clist_thaw(GTK_CLIST(search->ctree));
	g_slist_free(data_list);
	g_list_free(node_list);
}

/* -*- mode: cc-mode; tab-width:4; -*- */
/* vi: set ts=4 sw=4 cindent: */
