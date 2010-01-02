ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../../.. -I$(srcdir)/../../../include/rservr -I$(srcdir)/../../../include -I$(srcdir)/../../include/proto -I$(srcdir)/../local-include -I$(srcdir)/../../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL -DRSERVR_RESTRICT_COMMAND_INTERFACE $(ALL_INCLUDES)
lib_LTLIBRARIES=librservr-proto-resource.la
librservr_proto_resource_la_SOURCES=proto-resource-client.cpp
librservr_proto_resource_la_LIBADD=../../command/librservr-command.la
EXTRA_DIST=../../include/proto/proto-resource-client.hpp
