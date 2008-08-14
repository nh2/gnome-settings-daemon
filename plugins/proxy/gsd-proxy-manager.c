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

#include "config.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <locale.h>

#include <glib.h>
#include <glib/gi18n.h>
#include <gio/gio.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <gconf/gconf-client.h>

#include "gnome-settings-profile.h"
#include "gsd-proxy-manager.h"

#define GSD_PROXY_MANAGER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GSD_TYPE_PROXY_MANAGER, GsdProxyManagerPrivate))

/* Novell extension */
#define KEY_USE_SYSTEM_SETTINGS			"/system/proxy/use_system_settings"		/* string */
#define VAL_USE_SYSTEM_SETTINGS_ONLY_IF_NOT_SET	"only_if_mode_not_set"
#define VAL_USE_SYSTEM_SETTINGS_SYSTEM_VALUES	"system_values"
#define VAL_USE_SYSTEM_SETTINGS_USER_VALUES	"user_values"

#define ETC_SYSCONFIG_PROXY_URI			"file:///etc/sysconfig/proxy"

/* Gnome keys */
#define DIR_PROXY				"/system/proxy"
#define KEY_USE_HTTP_PROXY			"/system/http_proxy/use_http_proxy"		/* bool */
#define KEY_HTTP_HOST				"/system/http_proxy/host"			/* string */
#define KEY_HTTP_PORT				"/system/http_proxy/port"			/* int */
#define KEY_HTTP_USE_AUTHENTICATION		"/system/http_proxy/use_authentication"		/* bool */
#define KEY_HTTP_AUTHENTICATION_USER		"/system/http_proxy/authentication_user"	/* string */
#define KEY_HTTP_AUTHENTICATION_PASSWORD	"/system/http_proxy/authentication_password"	/* string */
#define KEY_HTTP_IGNORE_HOSTS			"/system/http_proxy/ignore_hosts"		/* list-of-string */
#define KEY_MODE				"/system/proxy/mode"				/* string */
#define KEY_SECURE_HOST				"/system/proxy/secure_host"			/* string */
#define KEY_SECURE_PORT				"/system/proxy/secure_port"			/* int */
#define KEY_FTP_HOST				"/system/proxy/ftp_host"			/* string */
#define KEY_FTP_PORT				"/system/proxy/ftp_port"			/* int */
#define KEY_SOCKS_HOST				"/system/proxy/socks_host"			/* string */
#define KEY_SOCKS_PORT				"/system/proxy/socks_port"			/* int */
#define KEY_AUTOCONFIG_URL			"/system/proxy/autoconfig_url"			/* string */

struct GsdProxyManagerPrivate
{
};

typedef struct {
        char *proxy_enabled;	/* "yes"/"no" */
        char *http_proxy;	/* "http://host:port" */
        char *https_proxy;    /* "http://host:port" */
        char *ftp_proxy;	/* "http://host:port" */
        /* char *gopher_proxy; Gnome doesn't have a Gopher proxy setting as of 2.10 */
        char *no_proxy;	/* "www.me.de, do.main, localhost" */
        /* char *user;
           * char *password;
           * Yast2 currently doesn't support a public username/password; they are just
           * stored in /root/.wgetrc and /root/.curlrc
           */
} SystemSettings;

static void     gsd_proxy_manager_class_init  (GsdProxyManagerClass *klass);
static void     gsd_proxy_manager_init        (GsdProxyManager      *proxy_manager);
static void     gsd_proxy_manager_finalize    (GObject             *object);

G_DEFINE_TYPE (GsdProxyManager, gsd_proxy_manager, G_TYPE_OBJECT)

static gpointer manager_object = NULL;

/* Returns whether the /system/proxy/mode key has ever been set by the user */
static gboolean
user_mode_is_set (GConfClient *config_client)
{
	GConfValue *value;

	value = gconf_client_get_without_default (config_client, KEY_MODE, NULL);

	if (value)
	{
		gconf_value_free (value);
		return TRUE;
	}
	else
		return FALSE;
}

static const char *
until_newline (const char *str, char **dest)
{
	const char *p;

	p = strchr (str, '\n');
	if (!p)
		p = str + strlen (str);

	if (dest)
		*dest = g_strstrip (g_strndup (str, p - str));

	return p;
}

static const char *
scan_for_token_and_jump_until_newline (const char *buf, const char *token, char **dest)
{
	int len;

	len = strlen (token);

	if (strncmp (buf, token, len) == 0)
		return until_newline (buf + len, dest);
	else
		return buf;
}

/* Reads the system's proxy settings */
static void
read_system_settings (SystemSettings *settings)
{
	char *out;
	const char *p;
	int total_len;
	struct tokens {
		const char *token;
		char **dest;
	} tokens[] = {
		{ "PROXY_ENABLED ", &settings->proxy_enabled },
		{ "HTTP_PROXY ",    &settings->http_proxy },
		{ "HTTPS_PROXY ",   &settings->https_proxy },
		{ "FTP_PROXY ",     &settings->ftp_proxy },
		{ "NO_PROXY ",      &settings->no_proxy }
	};
	int num_tokens = G_N_ELEMENTS (tokens);

	settings->proxy_enabled = NULL;
	settings->http_proxy = NULL;
	settings->https_proxy = NULL;
	settings->ftp_proxy = NULL;
	settings->no_proxy = NULL;

	if (!g_spawn_command_line_sync (LIBEXECDIR "/novell-sysconfig-proxy-helper",
					&out,
					NULL,
					NULL,
					NULL))

		return;

	total_len = strlen (out);

	p = out;
	while (p < out + total_len)
	{
		int i;

		for (i = 0; i < num_tokens; i++)
			p = scan_for_token_and_jump_until_newline (p, tokens[i].token, tokens[i].dest);

		if (i == num_tokens)
			p = until_newline (p, NULL);

		if (*p == '\n')
			p++;
	}

	g_free (out);
}

static void
system_settings_free (SystemSettings *settings)
{
	g_free (settings->proxy_enabled);
	g_free (settings->http_proxy);
	g_free (settings->https_proxy);
	g_free (settings->ftp_proxy);
	g_free (settings->no_proxy);
}

/* Disables the proxy in the user's settings */
static void
copy_proxy_disabled (GConfClient *config_client)
{
	gconf_client_set_bool (config_client, KEY_USE_HTTP_PROXY, FALSE, NULL);
	gconf_client_set_string (config_client, KEY_MODE, "none", NULL);
}

/* parses a URI to get the host and port */
static gboolean
parse_uri (const gchar *uri, gchar **host, guint *port)
{
        char **tokens;
        
        tokens = g_strsplit_set (uri, ":", 0);
        if (tokens[1] != NULL) {
                *host = g_strdup (tokens[1] + 2);
        }
        
        if (tokens[2] != NULL) {
                *port = (guint) atoi (tokens[2]);
        }
        
        g_debug ("Proxy host: %s:%d", *host, *port);
        
        g_strfreev (tokens);
        return TRUE;
}

/* Copies the (enabled) system proxy settings in the user's settings */
static void
copy_proxy_enabled (GConfClient *config_client, SystemSettings *settings)
{
	gchar *host;
	guint port;

	gconf_client_set_string (config_client, KEY_MODE, "manual", NULL);

	/* 1. HTTP proxy */

	/* Yast2 currently doesn't support a public username/password */
	gconf_client_set_bool (config_client, KEY_HTTP_USE_AUTHENTICATION, FALSE, NULL);
	gconf_client_set_string (config_client, KEY_HTTP_AUTHENTICATION_USER, "", NULL);
	gconf_client_set_string (config_client, KEY_HTTP_AUTHENTICATION_PASSWORD, "", NULL);

	if (parse_uri (settings->http_proxy, &host, &port))
	{
		gconf_client_set_bool (config_client, KEY_USE_HTTP_PROXY, TRUE, NULL);

		gconf_client_set_string (config_client, KEY_HTTP_HOST, host, NULL);
		gconf_client_set_int (config_client, KEY_HTTP_PORT, (int) port, NULL);

		g_free (host);
	}
	else
	{
		gconf_client_set_bool (config_client, KEY_USE_HTTP_PROXY, FALSE, NULL);
		gconf_client_set_string (config_client, KEY_HTTP_HOST, "", NULL);
		gconf_client_set_int (config_client, KEY_HTTP_PORT, 0, NULL);
		gconf_client_set_list (config_client, KEY_HTTP_IGNORE_HOSTS, GCONF_VALUE_STRING, NULL, NULL);      
	}

	/* 2. HTTPS proxy */

	if (parse_uri (settings->https_proxy, &host, &port))
	{
		gconf_client_set_string (config_client, KEY_SECURE_HOST, host, NULL);
		gconf_client_set_int (config_client, KEY_SECURE_PORT, (int) port, NULL);

		g_free (host);
	}
	else
	{
		gconf_client_set_string (config_client, KEY_SECURE_HOST, "", NULL);
		gconf_client_set_int (config_client, KEY_SECURE_PORT, 0, NULL);
	}

	/* 3. FTP proxy */

	if (parse_uri (settings->ftp_proxy, &host, &port))
	{
		gconf_client_set_string (config_client, KEY_FTP_HOST, host, NULL);
		gconf_client_set_int (config_client, KEY_FTP_PORT, (int) port, NULL);

		g_free (host);
	}
	else
	{
		gconf_client_set_string (config_client, KEY_FTP_HOST, "", NULL);
		gconf_client_set_int (config_client, KEY_FTP_PORT, 0, NULL);
	}

	/* 4. No-proxy hosts */

	if (settings->no_proxy != NULL)
	{
		char **tokens;
		int i;
		GSList *list;

		tokens = g_strsplit_set (settings->no_proxy, ", ", 0);

		list = NULL;

		for (i = 0; tokens[i] != NULL; i++)
		{
			char *s;

			s = tokens[i];
			if (strlen (s) != 0)
				list = g_slist_prepend (list, s);
		}

		list = g_slist_reverse (list);

		gconf_client_set_list (config_client, KEY_HTTP_IGNORE_HOSTS, GCONF_VALUE_STRING, list, NULL);

		g_slist_free (list);
		g_strfreev (tokens);
	}
	else
		gconf_client_set_list (config_client, KEY_HTTP_IGNORE_HOSTS, GCONF_VALUE_STRING, NULL, NULL);
}

/* Copies the system's proxy settings to the user's settings */
static void
copy_system_to_user (GConfClient *config_client)
{
	SystemSettings settings;

	read_system_settings (&settings);

	if (settings.proxy_enabled == NULL)
		copy_proxy_disabled (config_client);
	else
	{
		if (strcmp (settings.proxy_enabled, "no") == 0)
			copy_proxy_disabled (config_client);
		else if (strcmp (settings.proxy_enabled, "yes") == 0)
			copy_proxy_enabled (config_client, &settings);
	}

	system_settings_free (&settings);
}

static void
use_system_settings_change (GConfClient *config_client, const char *value)
{
	if (strcmp (value, VAL_USE_SYSTEM_SETTINGS_SYSTEM_VALUES) == 0)
	{
		copy_system_to_user (config_client);
	}
	else if (strcmp (value, VAL_USE_SYSTEM_SETTINGS_USER_VALUES) == 0)
	{
		/* nothing; the user's values are already set */
	}
}

static void
dir_proxy_key_changed_cb (GConfEntry *entry)
{
	GConfClient *client = gconf_client_get_default ();
	
	if (strcmp (entry->key, KEY_USE_SYSTEM_SETTINGS) == 0)
		use_system_settings_change (client, gconf_value_get_string (entry->value));
        
        g_object_unref (client);
}

static void
copy_system_values_if_needed (GConfClient *client)
{
	char *value;

	value = gconf_client_get_string (client, KEY_USE_SYSTEM_SETTINGS, NULL);
	if (!value)
		return;

	use_system_settings_change (client, value);
	g_free (value);
}

/* File monitor callback for /etc/sysconfig/proxy */
static void
monitor_cb (GFileMonitor *monitor,
	    GFile *file,
	    GFile *other_file,
	    GFileMonitorEvent event_type,
	    gpointer data)
{
        GConfClient *client;

	if (event_type != G_FILE_MONITOR_EVENT_CHANGED)
		return;

        client = gconf_client_get_default ();
	copy_system_values_if_needed (client);
	g_object_unref (G_OBJECT (client));
}

gboolean
gsd_proxy_manager_start (GsdProxyManager *manager,
                         GError **error)
{
        GConfClient *config_client;
        GFile *sysconfig_proxy;
        GFileMonitor *monitor;
	char *use_system_settings;

        g_debug ("Starting proxy manager");
        gnome_settings_profile_start (NULL);

        config_client = gconf_client_get_default ();
        
        /* The very first time g-s-d is run, we change the user's keys if the new
	 * use_system_settings key is not set at all or is set to the default.
	 * Afterwards, that key will be set to reflect what the user had previously
	 * configured.
	 */

	use_system_settings = gconf_client_get_string (config_client, KEY_USE_SYSTEM_SETTINGS, NULL);
	if (!use_system_settings || strcmp (use_system_settings, VAL_USE_SYSTEM_SETTINGS_ONLY_IF_NOT_SET) == 0)
	{
		if (user_mode_is_set (config_client))
			gconf_client_set_string (config_client, KEY_USE_SYSTEM_SETTINGS, VAL_USE_SYSTEM_SETTINGS_USER_VALUES, NULL);
		else
		{
			copy_system_to_user (config_client);
			gconf_client_set_string (config_client, KEY_USE_SYSTEM_SETTINGS, VAL_USE_SYSTEM_SETTINGS_SYSTEM_VALUES, NULL);
		}
	}

	if (use_system_settings)
		g_free (use_system_settings);

        gconf_client_add_dir (config_client, DIR_PROXY, GCONF_CLIENT_PRELOAD_NONE, NULL);
        gconf_client_notify_add (config_client, DIR_PROXY, dir_proxy_key_changed_cb, NULL, NULL, NULL);

        sysconfig_proxy = g_file_new_for_uri (ETC_SYSCONFIG_PROXY_URI);
        monitor = g_file_monitor_file (sysconfig_proxy, 0, NULL, NULL);
        g_signal_connect (monitor, "changed", G_CALLBACK (monitor_cb), NULL);
        
        copy_system_values_if_needed (config_client);

        g_object_unref (config_client);
        
        gnome_settings_profile_end (NULL);

        return TRUE;
}

void
gsd_proxy_manager_stop (GsdProxyManager *manager)
{
        g_debug ("Stopping proxy manager");
}

static void
gsd_proxy_manager_set_property (GObject        *object,
                               guint           prop_id,
                               const GValue   *value,
                               GParamSpec     *pspec)
{
        GsdProxyManager *self;

        self = GSD_PROXY_MANAGER (object);

        switch (prop_id) {
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
                break;
        }
}

static void
gsd_proxy_manager_get_property (GObject        *object,
                               guint           prop_id,
                               GValue         *value,
                               GParamSpec     *pspec)
{
        GsdProxyManager *self;

        self = GSD_PROXY_MANAGER (object);

        switch (prop_id) {
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
                break;
        }
}

static GObject *
gsd_proxy_manager_constructor (GType                  type,
                              guint                  n_construct_properties,
                              GObjectConstructParam *construct_properties)
{
        GsdProxyManager      *proxy_manager;
        GsdProxyManagerClass *klass;

        klass = GSD_PROXY_MANAGER_CLASS (g_type_class_peek (GSD_TYPE_PROXY_MANAGER));

        proxy_manager = GSD_PROXY_MANAGER (G_OBJECT_CLASS (gsd_proxy_manager_parent_class)->constructor (type,
                                                                                                      n_construct_properties,
                                                                                                      construct_properties));

        return G_OBJECT (proxy_manager);
}

static void
gsd_proxy_manager_dispose (GObject *object)
{
        GsdProxyManager *proxy_manager;

        proxy_manager = GSD_PROXY_MANAGER (object);

        G_OBJECT_CLASS (gsd_proxy_manager_parent_class)->dispose (object);
}

static void
gsd_proxy_manager_class_init (GsdProxyManagerClass *klass)
{
        GObjectClass   *object_class = G_OBJECT_CLASS (klass);

        object_class->get_property = gsd_proxy_manager_get_property;
        object_class->set_property = gsd_proxy_manager_set_property;
        object_class->constructor = gsd_proxy_manager_constructor;
        object_class->dispose = gsd_proxy_manager_dispose;
        object_class->finalize = gsd_proxy_manager_finalize;

        g_type_class_add_private (klass, sizeof (GsdProxyManagerPrivate));
}

static void
gsd_proxy_manager_init (GsdProxyManager *manager)
{
        manager->priv = GSD_PROXY_MANAGER_GET_PRIVATE (manager);
}

static void
gsd_proxy_manager_finalize (GObject *object)
{
        GsdProxyManager *proxy_manager;

        g_return_if_fail (object != NULL);
        g_return_if_fail (GSD_IS_PROXY_MANAGER (object));

        proxy_manager = GSD_PROXY_MANAGER (object);

        g_return_if_fail (proxy_manager->priv != NULL);

        G_OBJECT_CLASS (gsd_proxy_manager_parent_class)->finalize (object);
}

GsdProxyManager *
gsd_proxy_manager_new (void)
{
        if (manager_object != NULL) {
                g_object_ref (manager_object);
        } else {
                manager_object = g_object_new (GSD_TYPE_PROXY_MANAGER, NULL);
                g_object_add_weak_pointer (manager_object,
                                           (gpointer *) &manager_object);
        }

        return GSD_PROXY_MANAGER (manager_object);
}
