AM_LDFLAGS=-version-info 4:0:0
plugindir=$(PLUGIN_PATH)
ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../.. -I$(srcdir)/../../include/rservr -I$(srcdir)/../../include -I$(srcdir)/../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL -DRSERVR_RESTRICT_COMMAND_INTERFACE $(ALL_INCLUDES)
plugin_LTLIBRARIES=librsvp-passthru.la librsvp-passthru-assist.la
librsvp_passthru_la_SOURCES=api-passthru.cpp plugin-passthru.cpp api-passthru.h plugin-passthru.hpp
librsvp_passthru_la_LIBADD=../../libs/protocol/librservr-protocol.la
librsvp_passthru_assist_la_SOURCES=passthru-assist.cpp
librsvp_passthru_assist_la_LIBADD=librsvp-passthru.la ../../libs/client/librservr-client.la
