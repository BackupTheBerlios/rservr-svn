AM_LDFLAGS=-version-info 3:0:3
ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../../.. -I$(srcdir)/../../../include/rservr -I$(srcdir)/../../../include -I$(srcdir)/../../include/proto -I$(srcdir)/../local-include -I$(srcdir)/../../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL -DRSERVR_RESTRICT_COMMAND_INTERFACE $(ALL_INCLUDES)
lib_LTLIBRARIES=librservr-proto-server.la
librservr_proto_server_la_SOURCES=proto-server.cpp
librservr_proto_server_la_LIBADD=../../protocol/librservr-protocol.la
EXTRA_DIST=../../include/proto/proto-server.hpp
