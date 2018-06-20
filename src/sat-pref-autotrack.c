/*
    Gpredict: Real-time satellite tracking and orbit prediction program

    Copyright (C)  2001-2017  Alexandru Csete, OZ9AEC.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
  
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
  
    You should have received a copy of the GNU General Public License
    along with this program; if not, visit http://www.fsf.org/
*/
#ifdef HAVE_CONFIG_H
#include <build-config.h>
#endif
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include "config-keys.h"
#include "mod-cfg-get-param.h"
#include "sat-cfg.h"
#include "sat-pref-autotrack.h"


static GtkWidget *min_el_spin;     /* spin button for module refresh rate */

static gboolean dirty = FALSE;  /* used to check whether any changes have occurred */
static gboolean reset = FALSE;


/* User pressed cancel. Any changes to config must be cancelled. */
void sat_pref_autotrack_cancel(GKeyFile * cfg)
{
    (void)cfg;

    dirty = FALSE;
}

/* User pressed OK. Any changes should be stored in config. */
void sat_pref_autotrack_ok(GKeyFile * cfg)
{
    if (dirty)
    {
        if (cfg != NULL)
        {
            g_key_file_set_integer(cfg,
                                   MOD_CFG_AUTOTRACK_SECTION,
                                   MOD_CFG_AUTOTRACK_MIN_EL,
                                   gtk_spin_button_get_value_as_int
                                   (GTK_SPIN_BUTTON(min_el_spin)));
        }
        else
        {
            sat_cfg_set_int(SAT_CFG_INT_AUTOTRACK_MIN_EL,
                            gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
                                                             (min_el_spin)));
        }
    }
    else if (reset)
    {
        /* we have to reset the values to global or default settings */
        if (cfg == NULL)
        {
            /* reset values in sat-cfg */
            sat_cfg_reset_int(SAT_CFG_INT_AUTOTRACK_MIN_EL);

        }
        else
        {
            /* remove keys */
            g_key_file_remove_key((GKeyFile *) (cfg),
                                  MOD_CFG_AUTOTRACK_SECTION,
                                  MOD_CFG_AUTOTRACK_MIN_EL, NULL);
        }
    }

    dirty = FALSE;
    reset = FALSE;
}

static void spin_changed_cb(GtkWidget * spinner, gpointer data)
{
    (void)spinner;
    (void)data;

    dirty = TRUE;
}

/**
 * Reset settings.
 *
 * @param button The RESET button.
 * @param cfg Pointer to the module config or NULL in global mode.
 *
 * This function is called when the user clicks on the RESET button. In global mode
 * (when cfg = NULL) the function will reset the settings to the efault values, while
 * in "local" mode (when cfg != NULL) the function will reset the module settings to
 * the global settings. This is done by removing the corresponding key from the GKeyFile.
 */
static void reset_cb(GtkWidget * button, gpointer cfg)
{
    gint            val;

    (void)button;

    /* views */
    if (cfg == NULL)
    {
        /* global mode, get defaults */
        val = sat_cfg_get_int_def(SAT_CFG_INT_AUTOTRACK_MIN_EL);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(min_el_spin), val);

    }
    else
    {
        /* local mode, get global value */
        val = sat_cfg_get_int(SAT_CFG_INT_AUTOTRACK_MIN_EL);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(min_el_spin), val);
    }

    /* reset flags */
    reset = TRUE;
    dirty = FALSE;
}

static void create_reset_button(GKeyFile * cfg, GtkBox * vbox)
{
    GtkWidget      *button;
    GtkWidget      *butbox;

    button = gtk_button_new_with_label(_("Reset"));
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(reset_cb), cfg);

    if (cfg == NULL)
    {
        gtk_widget_set_tooltip_text(button,
                                    _
                                    ("Reset settings to the default values."));
    }
    else
    {
        gtk_widget_set_tooltip_text(button,
                                    _
                                    ("Reset module settings to the global values."));
    }

    butbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(butbox), GTK_BUTTONBOX_END);
    gtk_box_pack_end(GTK_BOX(butbox), button, FALSE, TRUE, 10);

    gtk_box_pack_end(vbox, butbox, FALSE, TRUE, 0);

}

GtkWidget      *sat_pref_autotrack_create(GKeyFile * cfg)
{
    GtkWidget      *table;
    GtkWidget      *vbox;
    GtkWidget      *label;
    gint            val;

    dirty = FALSE;
    reset = FALSE;

    table = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(table), 10);
    gtk_grid_set_column_spacing(GTK_GRID(table), 5);

    /* Minimum Tracking elevation */
    label = gtk_label_new(_("Minimum Elevation for valid targets"));
    g_object_set(label, "xalign", 0.0, "yalign", 0.5, NULL);
    gtk_grid_attach(GTK_GRID(table), label, 0, 0, 1, 1);

    min_el_spin = gtk_spin_button_new_with_range(0, 180, 1);
    gtk_spin_button_set_increments(GTK_SPIN_BUTTON(min_el_spin), 1, 100);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(min_el_spin), TRUE);
    gtk_spin_button_set_update_policy(GTK_SPIN_BUTTON(min_el_spin),
                                      GTK_UPDATE_IF_VALID);
    if (cfg != NULL)
    {
        val = mod_cfg_get_int(cfg,
                              MOD_CFG_AUTOTRACK_SECTION,
                              MOD_CFG_AUTOTRACK_MIN_EL, SAT_CFG_INT_AUTOTRACK_MIN_EL);
    }
    else
    {
        val = sat_cfg_get_int(SAT_CFG_INT_AUTOTRACK_MIN_EL);
    }
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(min_el_spin), val);
    g_signal_connect(G_OBJECT(min_el_spin), "value-changed",
                     G_CALLBACK(spin_changed_cb), NULL);
    gtk_grid_attach(GTK_GRID(table), min_el_spin, 1, 0, 1, 1);

    label = gtk_label_new(_("[degrees]"));
    g_object_set(label, "xalign", 0.0, "yalign", 0.5, NULL);
    gtk_grid_attach(GTK_GRID(table), label, 2, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(table),
                    gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), 0, 1, 3, 1);

    /* create vertical box */
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(vbox), FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);
    gtk_box_pack_start(GTK_BOX(vbox), table, TRUE, TRUE, 0);

    /* create RESET button */
    create_reset_button(cfg, GTK_BOX(vbox));

    return vbox;
}
