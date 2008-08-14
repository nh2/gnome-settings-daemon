NULL =

plugin_LTLIBRARIES = \
	libproxy.la  \
	$(NULL)

libproxy_la_SOURCES = 		\
	gsd-proxy-plugin.h	\
	gsd-proxy-plugin.c	\
	gsd-proxy-manager.h	\
	gsd-proxy-manager.c	\
	$(NULL)

libproxy_la_CPPFLAGS = \
	-I$(top_srcdir)/gnome-settings-daemon		\
	-DGNOME_SETTINGS_LOCALEDIR=\""$(datadir)/locale"\" \
	$(AM_CPPFLAGS)

libproxy_la_CFLAGS = 				\
	$(SETTINGS_PLUGIN_CFLAGS)		\
	-DLIBEXECDIR="\"$(libexecdir)\""	\
	$(AM_CFLAGS)

libproxy_la_LDFLAGS = 		\
	$(GSD_PLUGIN_LDFLAGS)	\
	$(NULL)

libproxy_la_LIBADD  = 		\
	$(SETTINGS_PLUGIN_LIBS)	\
	$(NULL)

plugin_in_files = 		\
	proxy.gnome-settings-plugin.in	\
	$(NULL)

plugin_DATA = $(plugin_in_files:.gnome-settings-plugin.in=.gnome-settings-plugin)

helperdir = $(libexecdir)
helperfile = novell-sysconfig-proxy-helper

install-data-hook :
	$(INSTALL_SCRIPT) $(srcdir)/$(helperfile) $(DESTDIR)$(libexecdir)/$(helperfile)

EXTRA_DIST = 			\
	$(plugin_in_files)	\
	$(helperfile)		\
	$(NULL)

CLEANFILES = 			\
	$(plugin_DATA)		\
	$(NULL)

DISTCLEANFILES =		\
	$(plugin_DATA)		\
	$(NULL)

@GSD_INTLTOOL_PLUGIN_RULE@
