if LINK_LIBPTHREAD
  USE_LIBPTHREAD=-lpthread
endif

ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../../.. -I$(srcdir)/../../../include/rservr -I$(srcdir)/../../../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL $(ALL_INCLUDES)
bin_PROGRAMS=rservrd
rservrd_SOURCES=daemon-commands.c daemon-socket.c daemon.c interface.c rservrd.c daemon-regex.cpp commands/general.c commands/clients.c commands/control.c commands/netcntl.c
rservrd_LDADD=$(STANDARD_LIBRARIES) $(USE_LIBPTHREAD) ../../../plugins/rsvp-netcntl/librsvp-netcntl.la ../../../plugins/rsvp-rqconfig/librsvp-rqconfig.la ../../../libs/client/librservr-client.la
EXTRA_DIST=NOTICE daemon-commands.h daemon-socket.h daemon.h interface.h daemon-regex.h commands/include/commands-common.h commands/include/general.h commands/include/clients.h commands/include/control.h commands/include/netcntl.h

RSERVRD=rservrd

install-exec-hook:
	if test -d $(RSERVRD_HOME); then \
	  chmod -v 1777 $(RSERVRD_HOME); \
	else \
	  mkdir -v -m 1777 $(RSERVRD_HOME); \
	fi;


uninstall-hook:
	rmdir $(RSERVRD_HOME) || true
