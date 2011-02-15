AM_LDFLAGS=-version-info 4:0:0
ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../../.. -I$(srcdir)/../../../include/rservr -I$(srcdir)/../../../include -I$(srcdir)/../../include/proto -I$(srcdir)/../local-include -I$(srcdir)/../../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL -DRSERVR_RESTRICT_COMMAND_INTERFACE $(ALL_INCLUDES)
lib_LTLIBRARIES=librservr-proto-admin.la
librservr_proto_admin_la_SOURCES=proto-admin-client.cpp
librservr_proto_admin_la_LIBADD=../../protocol/librservr-protocol.la
EXTRA_DIST=../../include/proto/proto-admin-client.hpp
