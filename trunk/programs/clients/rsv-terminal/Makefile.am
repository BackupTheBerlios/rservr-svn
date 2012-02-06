ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../../.. -I$(srcdir)/../../../include/rservr -I$(srcdir)/../../../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL $(ALL_INCLUDES)
bin_PROGRAMS=rsv-terminal rsv-console rsv-messages

if LINK_READLINE
  USE_READLINE=-lreadline -lncurses
endif

if LINK_HISTORY
  USE_HISTORY=-lhistory
endif

rsv_terminal_CPPFLAGS=$(ALL_INCLUDES) -DRSV_TERMINAL
rsv_terminal_SOURCES=rsv-terminal.c terminal.c interpreter.c commands.cpp terminal.h interpreter.h commands.h
rsv_terminal_LDADD=../../../plugins/rsvp-rqsrvc/librsvp-rqsrvc.la ../../../plugins/rsvp-netcntl/librsvp-netcntl.la ../../../libs/client/librservr-client.la

rsv_console_CPPFLAGS=$(ALL_INCLUDES) -DRSV_CONSOLE
rsv_console_SOURCES=rsv-terminal.c terminal.c interpreter.c commands.cpp terminal.h interpreter.h commands.h
rsv_console_LDADD=$(STANDARD_LIBRARIES) $(USE_READLINE) $(USE_HISTORY) ../../../plugins/rsvp-rqsrvc/librsvp-rqsrvc.la ../../../plugins/rsvp-netcntl/librsvp-netcntl.la ../../../libs/client/librservr-client.la

rsv_messages_CPPFLAGS=$(ALL_INCLUDES) -DRSV_MESSAGES
rsv_messages_SOURCES=rsv-terminal.c terminal.c socket-mutex.cpp terminal.h socket-mutex.h
rsv_messages_LDADD=$(STANDARD_LIBRARIES) ../../../libs/client/librservr-client.la
