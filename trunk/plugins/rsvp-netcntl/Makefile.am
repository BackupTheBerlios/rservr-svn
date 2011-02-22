AM_LDFLAGS=-version-info 4:0:0
plugindir=$(PLUGIN_PATH)
ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../.. -I$(srcdir)/../../include/rservr -I$(srcdir)/../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL -DRSERVR_RESTRICT_COMMAND_INTERFACE $(ALL_INCLUDES)
plugin_LTLIBRARIES=librsvp-netcntl.la
librsvp_netcntl_la_SOURCES=api-netcntl.cpp plugin-netcntl.cpp api-netcntl.h plugin-netcntl.hpp
librsvp_netcntl_la_LIBADD=../../libs/protocol/librservr-protocol.la
