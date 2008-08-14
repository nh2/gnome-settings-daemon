/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2008 Rodrigo Moya <rodrigo@gnome-db.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#ifndef __GSD_PROXY_MANAGER_H
#define __GSD_PROXY_MANAGER_H

#include <glib-object.h>

G_BEGIN_DECLS

#define GSD_TYPE_PROXY_MANAGER         (gsd_proxy_manager_get_type ())
#define GSD_PROXY_MANAGER(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), GSD_TYPE_PROXY_MANAGER, GsdProxyManager))
#define GSD_PROXY_MANAGER_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), GSD_TYPE_PROXY_MANAGER, GsdProxyManagerClass))
#define GSD_IS_PROXY_MANAGER(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), GSD_TYPE_PROXY_MANAGER))
#define GSD_IS_PROXY_MANAGER_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), GSD_TYPE_PROXY_MANAGER))
#define GSD_PROXY_MANAGER_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), GSD_TYPE_PROXY_MANAGER, GsdProxyManagerClass))

typedef struct GsdProxyManagerPrivate GsdProxyManagerPrivate;

typedef struct
{
        GObject                     parent;
        GsdProxyManagerPrivate *priv;
} GsdProxyManager;

typedef struct
{
        GObjectClass   parent_class;
} GsdProxyManagerClass;

GType                   gsd_proxy_manager_get_type            (void);

GsdProxyManager *       gsd_proxy_manager_new                 (void);
gboolean                gsd_proxy_manager_start               (GsdProxyManager *manager,
                                                               GError         **error);
void                    gsd_proxy_manager_stop                (GsdProxyManager *manager);

G_END_DECLS

#endif /* __GSD_PROXY_MANAGER_H */
