STANDARD_INCLUDES=-I/usr/include -I/usr/local/include
ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../../.. -I$(srcdir)/../../../include/rservr -I$(srcdir)/../../../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL $(ALL_INCLUDES)
AM_CXXFLAGS=-ansi -Wall
AM_CFLAGS=-Wall
AM_LDFLAGS=
bin_PROGRAMS=rservrd
rservrd_SOURCES=daemon-commands.c daemon-socket.c daemon.c interface.c rservrd.c commands/general.c commands/clients.c commands/control.c commands/netcntl.c static-auto-conditions.cpp daemon-commands.h daemon-socket.h daemon.h interface.h commands/include/commands-common.h commands/include/general.h commands/include/clients.h commands/include/control.h commands/include/netcntl.h static-auto-conditions.h
rservrd_LDADD=-lpthread ../../../plugins/netcntl/librsvp-netcntl.la ../../../plugins/rqconfig/librsvp-rqconfig.la ../../../libs/client/librservr-client.la
EXTRA_DIST=NOTICE

RSERVRD=rservrd
RSERVRD_TABLE=/tmp/$(RSERVRD)
RSERVRD_PERMISSION=6755

install-exec-hook:
	if [ "$$(id -u 2> /dev/null)" -eq 0 ]; then \
	  if ! ( id $(RSERVRD) &> /dev/null ); then \
	    ( groupadd $(RSERVRD) || true ) &> /dev/null; \
	    if ! ( useradd -g $(RSERVRD) $(RSERVRD) &> /dev/null ); then \
	      ( echo "{fail point 1}" && cat $(srcdir)/NOTICE && sleep 2 ) 1>&2 || true; \
	    fi; \
	  fi; \
	  if ! ( chown $(RSERVRD):$(RSERVRD) $(DESTDIR)$(bindir)/$(RSERVRD) &> /dev/null ) || \
	     ! ( chmod $(RSERVRD_PERMISSION) $(DESTDIR)$(bindir)/$(RSERVRD) &> /dev/null ); then \
	    ( echo "{fail point 2}" && cat $(srcdir)/NOTICE && sleep 2 ) 1>&2 || true; \
	  fi; \
	else \
	  ( echo "{fail point 0}" && cat $(srcdir)/NOTICE && sleep 2 ) 1>&2 || true; \
	fi;


uninstall-hook:
	if [ "$$(id -u 2> /dev/null)" -eq 0 ]; then \
	  ( rm -rfv $(RSERVRD_TABLE) &> /dev/null ) || true; \
	  ( userdel $(RSERVRD) &> /dev/null ) || true; \
	  ( groupdel $(RSERVRD) &> /dev/null ) || true; \
	fi;
