/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
    Gpredict: Real-time satellite tracking and orbit prediction program

    Copyright (C)  2001-2007  Alexandru Csete.

    Authors: Alexandru Csete <oz9aec@gmail.com>

    Comments, questions and bugreports should be submitted via
    http://sourceforge.net/projects/groundstation/
    More details can be found at the project home page:

            http://groundstation.sourceforge.net/
 
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

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "sat-log.h"
#include "compat.h"

#include "radio-conf.h"

#define GROUP           "Radio"
#define KEY_HOST        "Host"


/** \brief Read radio configuration.
 * \param conf Pointer to a radio_conf_t structure where the data will be
 *             stored.
 * 
 * This function reads a radio configuration from a .rig file into conf.
 * conf->name must contain the file name of the configuration (no path, just
 * file name).
 */
gboolean radio_conf_read (radio_conf_t *conf)
{
    GKeyFile *cfg = NULL;
    gchar    *confdir;
    gchar    *fname;
    
    
    if (conf->name == NULL)
        return FALSE;
    
    confdir = get_conf_dir();
    fname = g_strconcat (confdir, G_DIR_SEPARATOR_S,
                         "hwconf", G_DIR_SEPARATOR_S,
                         conf->name, ".rig", NULL);
    g_free (confdir);
    
    /* open .grc file */
    cfg = g_key_file_new ();
    g_key_file_load_from_file(cfg, fname, 0, NULL);
    
    if (cfg == NULL) {
        sat_log_log (SAT_LOG_LEVEL_ERROR,
                     _("%s: Could not load file %s\n"),
                     __FUNCTION__, fname);
        g_free (fname);
        
        return FALSE;
    }
    
    g_free (fname);
    
    /* read parameters */
    conf->host = g_key_file_get_string (cfg, GROUP, KEY_HOST, NULL);
    
    g_key_file_free (cfg);
    
    return TRUE;
}


/** \brief Save radio configuration.
 * \param conf Pointer to the radio configuration.
 * 
 * This function saves the radio configuration stored in conf to a
 * .rig file. conf->name must contain the file name of the configuration
 * (no path, just file name).
 */
void radio_conf_save (radio_conf_t *conf)
{
    GKeyFile *cfg = NULL;
    gchar    *confdir;
    gchar    *fname;
    gchar    *data;
    gsize     len;
    
    if (conf->name == NULL)
        return;
    
    /* create a config structure */
    cfg = g_key_file_new();
    
    g_key_file_set_string (cfg, GROUP, KEY_HOST, conf->host);
    
    /* convert to text sdata */
    data = g_key_file_to_data (cfg, &len, NULL);
    
    confdir = get_conf_dir();
    fname = g_strconcat (confdir, G_DIR_SEPARATOR_S,
                         "hwconf", G_DIR_SEPARATOR_S,
                         conf->name, ".rig", NULL);
    g_free (confdir);
        
    g_file_set_contents (fname, data, len, NULL);
    
    g_free (fname);
    g_free (data);
    g_key_file_free (cfg);
}
