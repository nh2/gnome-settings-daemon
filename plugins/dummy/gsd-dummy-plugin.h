/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2007 William Jon McCann <mccann@jhu.edu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#ifndef __GSD_DUMMY_PLUGIN_H__
#define __GSD_DUMMY_PLUGIN_H__

#include <glib.h>
#include <glib-object.h>
#include <gmodule.h>

#include "gnome-settings-plugin.h"

G_BEGIN_DECLS

#define GSD_TYPE_DUMMY_PLUGIN                (gsd_dummy_plugin_get_type ())
#define GSD_DUMMY_PLUGIN(o)                  (G_TYPE_CHECK_INSTANCE_CAST ((o), GSD_TYPE_DUMMY_PLUGIN, GsdDummyPlugin))
#define GSD_DUMMY_PLUGIN_CLASS(k)            (G_TYPE_CHECK_CLASS_CAST((k), GSD_TYPE_DUMMY_PLUGIN, GsdDummyPluginClass))
#define GSD_IS_DUMMY_PLUGIN(o)               (G_TYPE_CHECK_INSTANCE_TYPE ((o), GSD_TYPE_DUMMY_PLUGIN))
#define GSD_IS_DUMMY_PLUGIN_CLASS(k)         (G_TYPE_CHECK_CLASS_TYPE ((k), GSD_TYPE_DUMMY_PLUGIN))
#define GSD_DUMMY_PLUGIN_GET_CLASS(o)        (G_TYPE_INSTANCE_GET_CLASS ((o), GSD_TYPE_DUMMY_PLUGIN, GsdDummyPluginClass))

typedef struct GsdDummyPluginPrivate GsdDummyPluginPrivate;

typedef struct
{
        GnomeSettingsPlugin    parent;
        GsdDummyPluginPrivate *priv;
} GsdDummyPlugin;

typedef struct
{
        GnomeSettingsPluginClass parent_class;
} GsdDummyPluginClass;

GType   gsd_dummy_plugin_get_type            (void) G_GNUC_CONST;

/* All the plugins must implement this function */
G_MODULE_EXPORT GType register_gnome_settings_plugin (GTypeModule *module);

G_END_DECLS

#endif /* __GSD_DUMMY_PLUGIN_H__ */
