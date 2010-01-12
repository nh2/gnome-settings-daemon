/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2001 Bastien Nocera <hadess@hadess.net>
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 */

#ifndef __ACME_H__
#define __ACME_H__

#include "gsd-keygrab.h"

#define GCONF_BINDING_DIR "/apps/gnome_settings_daemon/keybindings"
#define GCONF_MISC_DIR "/apps/gnome_settings_daemon"

enum {
        MUTE_KEY,
        VOLUME_DOWN_KEY,
        VOLUME_UP_KEY,
        POWER_KEY,
        EJECT_KEY,
        HOME_KEY,
        MEDIA_KEY,
        CALCULATOR_KEY,
        SEARCH_KEY,
        EMAIL_KEY,
        SLEEP_KEY,
        SCREENSAVER_KEY,
        HELP_KEY,
        WWW_KEY,
        PLAY_KEY,
        PAUSE_KEY,
        STOP_KEY,
        PREVIOUS_KEY,
        NEXT_KEY,
        HANDLED_KEYS
};

static struct {
        int key_type;
        const char *gconf_key;
        Key *key;
	char *hk_cond_detail;
} keys[HANDLED_KEYS] = {
        { MUTE_KEY, GCONF_BINDING_DIR "/volume_mute",NULL, NULL},
        { VOLUME_DOWN_KEY, GCONF_BINDING_DIR "/volume_down", NULL },
        { VOLUME_UP_KEY, GCONF_BINDING_DIR "/volume_up", NULL, NULL },
        { POWER_KEY, GCONF_BINDING_DIR "/power", NULL, NULL },
        { EJECT_KEY, GCONF_BINDING_DIR "/eject", NULL, NULL },
        { HOME_KEY, GCONF_BINDING_DIR "/home", NULL, NULL },
        { MEDIA_KEY, GCONF_BINDING_DIR "/media", NULL },
        { CALCULATOR_KEY, GCONF_BINDING_DIR "/calculator", NULL, NULL },
        { SEARCH_KEY, GCONF_BINDING_DIR "/search", NULL, NULL },
        { EMAIL_KEY, GCONF_BINDING_DIR "/email", NULL, NULL },
        { SLEEP_KEY, GCONF_BINDING_DIR "/sleep", NULL, NULL },
        { SCREENSAVER_KEY, GCONF_BINDING_DIR "/screensaver", NULL, NULL },
        { HELP_KEY, GCONF_BINDING_DIR "/help", NULL, NULL },
        { WWW_KEY, GCONF_BINDING_DIR "/www", NULL, NULL },
        { PLAY_KEY, GCONF_BINDING_DIR "/play", NULL, NULL },
        { PAUSE_KEY, GCONF_BINDING_DIR "/pause", NULL, NULL },
        { STOP_KEY, GCONF_BINDING_DIR "/stop", NULL, NULL },
        { PREVIOUS_KEY, GCONF_BINDING_DIR "/previous", NULL, NULL },
        { NEXT_KEY, GCONF_BINDING_DIR "/next", NULL, NULL },
};

#endif /* __ACME_H__ */
