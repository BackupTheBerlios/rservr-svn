STANDARD_INCLUDES=-I/usr/include -I/usr/local/include
ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../../.. -I$(srcdir)/../../../include/rservr -I$(srcdir)/../../../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL $(ALL_INCLUDES)
AM_CXXFLAGS=-ansi -Wall
AM_CFLAGS=-Wall
AM_LDFLAGS=
bin_PROGRAMS=rsv-terminal rsv-console

rsv_terminal_CPPFLAGS=$(ALL_INCLUDES)
rsv_terminal_SOURCES=rsv-terminal.c terminal.c interpreter.c commands.cpp terminal.h interpreter.h commands.h
rsv_terminal_LDADD=../../../plugins/rqsrvc/librsvp-rqsrvc.la ../../../plugins/netcntl/librsvp-netcntl.la ../../../libs/client/librservr-client.la

rsv_console_CPPFLAGS=$(ALL_INCLUDES) -DRSV_CONSOLE
rsv_console_SOURCES=rsv-terminal.c terminal.c interpreter.c commands.cpp terminal.h interpreter.h commands.h
rsv_console_LDADD=../../../plugins/rqsrvc/librsvp-rqsrvc.la ../../../plugins/netcntl/librsvp-netcntl.la ../../../libs/client/librservr-client.la
