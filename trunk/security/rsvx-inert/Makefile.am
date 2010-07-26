AM_LDFLAGS=-version-info 3:0:3
plugindir=$(PLUGIN_PATH)
ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../.. -I$(srcdir)/../../include/rservr
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL $(ALL_INCLUDES)
plugin_LTLIBRARIES=librsvx-inert.la
librsvx_inert_la_SOURCES=rsvx-inert.c
librsvx_inert_la_LIBADD=
