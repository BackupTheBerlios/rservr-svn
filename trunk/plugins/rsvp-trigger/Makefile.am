AM_LDFLAGS=-version-info 3:0:3
plugindir=$(PLUGIN_PATH)
ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../.. -I$(srcdir)/../../include/rservr -I$(srcdir)/../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL -DRSERVR_RESTRICT_COMMAND_INTERFACE $(ALL_INCLUDES)
plugin_LTLIBRARIES=librsvp-trigger.la
librsvp_trigger_la_SOURCES=api-trigger.cpp plugin-trigger.cpp api-trigger.h plugin-trigger.hpp
librsvp_trigger_la_LIBADD=../../libs/protocol/librservr-protocol.la
