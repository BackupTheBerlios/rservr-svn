if LINK_LIBPTHREAD
  USE_LIBPTHREAD=-lpthread
endif

ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../../.. -I$(srcdir)/../../../include/rservr -I$(srcdir)/../../../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL $(ALL_INCLUDES)
bin_PROGRAMS=rservrd
rservrd_SOURCES=daemon-commands.c daemon-socket.c daemon.c interface.c rservrd.c daemon-regex.cpp commands/general.c commands/clients.c commands/control.c commands/netcntl.c
rservrd_LDADD=$(STANDARD_LIBRARIES) $(USE_LIBPTHREAD) ../../../plugins/netcntl/librsvp-netcntl.la ../../../plugins/rqconfig/librsvp-rqconfig.la ../../../libs/client/librservr-client.la
EXTRA_DIST=NOTICE daemon-commands.h daemon-socket.h daemon.h interface.h daemon-regex.h commands/include/commands-common.h commands/include/general.h commands/include/clients.h commands/include/control.h commands/include/netcntl.h

RSERVRD=rservrd
RSERVRD_TABLE=/tmp/$(RSERVRD)
RSERVRD_PERMISSION=6755

install-exec-hook:
	if [ "$$(id -u 2> /dev/null)" -eq 0 ]; then \
	  case "$$(uname)" in \
	    "Linux"|"linux") \
	      if ! id $(RSERVRD); then \
	        groupadd $(RSERVRD) || true; \
	        if ! useradd -g $(RSERVRD) -d $(RSERVRD_TABLE) $(RSERVRD); then \
	          ( echo "{fail point 1[Linux]}" && cat $(srcdir)/NOTICE && sleep 2 ) 1>&2 || true; \
	        fi; \
	      fi; \
	    ;; \
	    "FreeBSD"|"freebsd") \
	      if ! id $(RSERVRD); then \
	        pw groupadd $(RSERVRD) || true; \
	        if ! pw useradd $(RSERVRD) -g $(RSERVRD) -d $(RSERVRD_TABLE); then \
	          ( echo "{fail point 1[FreeBSD]}" && cat $(srcdir)/NOTICE && sleep 2 ) 1>&2 || true; \
	        fi; \
	      fi; \
	    ;; \
	  esac; \
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
	  case "$$(uname)" in \
	    "Linux"|"linux") \
	      ( userdel $(RSERVRD) &> /dev/null ) || true; \
	      ( groupdel $(RSERVRD) &> /dev/null ) || true; \
	    ;; \
	    "FreeBSD"|"freebsd") \
	      ( pw userdel $(RSERVRD) &> /dev/null ) || true; \
	      ( pw groupdel $(RSERVRD) &> /dev/null ) || true; \
	    ;; \
	  esac; \
	fi;
