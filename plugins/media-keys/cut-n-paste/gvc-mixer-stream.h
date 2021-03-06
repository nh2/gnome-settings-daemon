/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2008 Red Hat, Inc.
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

#ifndef __GVC_MIXER_STREAM_H
#define __GVC_MIXER_STREAM_H

#include <glib-object.h>
#include <pulse/pulseaudio.h>

#include "gvc-channel-map.h"

G_BEGIN_DECLS

#define GVC_TYPE_MIXER_STREAM         (gvc_mixer_stream_get_type ())
#define GVC_MIXER_STREAM(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), GVC_TYPE_MIXER_STREAM, GvcMixerStream))
#define GVC_MIXER_STREAM_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), GVC_TYPE_MIXER_STREAM, GvcMixerStreamClass))
#define GVC_IS_MIXER_STREAM(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), GVC_TYPE_MIXER_STREAM))
#define GVC_IS_MIXER_STREAM_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), GVC_TYPE_MIXER_STREAM))
#define GVC_MIXER_STREAM_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), GVC_TYPE_MIXER_STREAM, GvcMixerStreamClass))

typedef struct GvcMixerStreamPrivate GvcMixerStreamPrivate;

typedef struct
{
        GObject                parent;
        GvcMixerStreamPrivate *priv;
} GvcMixerStream;

typedef struct
{
        GObjectClass           parent_class;

        /* vtable */
        gboolean (*change_volume)   (GvcMixerStream *stream,
                                     guint           volume);
        gboolean (*change_is_muted) (GvcMixerStream *stream,
                                     gboolean        is_muted);
} GvcMixerStreamClass;

GType               gvc_mixer_stream_get_type        (void);

pa_context *        gvc_mixer_stream_get_pa_context  (GvcMixerStream *stream);
guint               gvc_mixer_stream_get_index       (GvcMixerStream *stream);
guint               gvc_mixer_stream_get_id          (GvcMixerStream *stream);
GvcChannelMap *     gvc_mixer_stream_get_channel_map (GvcMixerStream *stream);

guint               gvc_mixer_stream_get_volume      (GvcMixerStream *stream);
gdouble             gvc_mixer_stream_get_decibel     (GvcMixerStream *stream);
gboolean            gvc_mixer_stream_change_volume   (GvcMixerStream *stream,
                                                      guint           volume);

gboolean            gvc_mixer_stream_get_is_muted    (GvcMixerStream *stream);
gboolean            gvc_mixer_stream_get_can_decibel (GvcMixerStream *stream);
gboolean            gvc_mixer_stream_change_is_muted (GvcMixerStream *stream,
                                                      gboolean        is_muted);
const char *        gvc_mixer_stream_get_name        (GvcMixerStream *stream);
const char *        gvc_mixer_stream_get_icon_name   (GvcMixerStream *stream);
const char *        gvc_mixer_stream_get_description (GvcMixerStream *stream);

/* private */
gboolean            gvc_mixer_stream_set_volume      (GvcMixerStream *stream,
                                                      guint           volume);
gboolean            gvc_mixer_stream_set_decibel     (GvcMixerStream *stream,
                                                      gdouble         db);
gboolean            gvc_mixer_stream_set_is_muted    (GvcMixerStream *stream,
                                                      gboolean        is_muted);
gboolean            gvc_mixer_stream_set_can_decibel (GvcMixerStream *stream,
                                                      gboolean        can_decibel);
gboolean            gvc_mixer_stream_set_name        (GvcMixerStream *stream,
                                                      const char     *name);
gboolean            gvc_mixer_stream_set_description (GvcMixerStream *stream,
                                                      const char     *description);
gboolean            gvc_mixer_stream_set_icon_name   (GvcMixerStream *stream,
                                                      const char     *name);

G_END_DECLS

#endif /* __GVC_MIXER_STREAM_H */
