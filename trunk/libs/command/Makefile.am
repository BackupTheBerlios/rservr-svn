AM_LDFLAGS=-version-info 3:0:3
ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../.. -I$(srcdir)/../../include/rservr -I$(srcdir)/../../include -I$(srcdir)/../include/command -I$(srcdir)/../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL $(ALL_INCLUDES)
lib_LTLIBRARIES=librservr-command.la
librservr_command_la_SOURCES=api-command.cpp command-generator.cpp api-label-check.cpp
librservr_command_la_LIBADD=-lhparser -lhpaux ../protocol/librservr-protocol.la
EXTRA_DIST=../include/command/api-command.h ../include/command/auto-response.hpp ../include/command/command-generator.hpp ../include/command/api-label-check.h
