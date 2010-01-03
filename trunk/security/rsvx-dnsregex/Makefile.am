plugindir=$(PLUGIN_PATH)
ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../.. -I$(srcdir)/../../include/rservr -I$(srcdir)/../../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL $(ALL_INCLUDES)
plugin_LTLIBRARIES=librsvx-dnsregex.la
librsvx_dnsregex_la_SOURCES=rsvx-dnsregex.cpp
librsvx_dnsregex_la_LIBADD=
