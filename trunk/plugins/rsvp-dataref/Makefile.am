AM_LDFLAGS=-version-info 4:0:0
plugindir=$(PLUGIN_PATH)
ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../.. -I$(srcdir)/../../include/rservr -I$(srcdir)/../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL -DRSERVR_RESTRICT_COMMAND_INTERFACE $(ALL_INCLUDES)
plugin_LTLIBRARIES=librsvp-dataref.la
librsvp_dataref_la_SOURCES=api-dataref.cpp plugin-dataref.cpp api-dataref-thread.cpp api-dataref.h plugin-dataref.hpp
librsvp_dataref_la_LIBADD=../../libs/protocol/librservr-protocol.la
