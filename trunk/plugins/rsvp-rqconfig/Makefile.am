AM_LDFLAGS=-version-info 4:0:0
plugindir=$(PLUGIN_PATH)
ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../.. -I$(srcdir)/../../include/rservr -I$(srcdir)/../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL -DRSERVR_RESTRICT_COMMAND_INTERFACE $(ALL_INCLUDES)
plugin_LTLIBRARIES=librsvp-rqconfig.la
librsvp_rqconfig_la_SOURCES=api-rqconfig.cpp plugin-rqconfig.cpp api-rqconfig-thread.cpp api-rqconfig.h plugin-rqconfig.hpp
librsvp_rqconfig_la_LIBADD=../../libs/protocol/librservr-protocol.la
