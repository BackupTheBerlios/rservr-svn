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
RSERVRD_PERMISSION=6755

install-exec-hook:
	if [ "$$(id -u 2> /dev/null)" -eq 0 ]; then \
	  case "$$(uname)" in \
	    "Linux"|"linux") \
	      if ! id $(RSERVRD_USER); then \
	        groupadd -g $(RSERVRD_GROUP) || true; \
	        if ! useradd -g $(RSERVRD_GROUP) -d $(RSERVRD_TABLE) -s /bin/false $(RSERVRD_USER); then \
	          ( echo "{fail point 1[Linux]}" && cat $(srcdir)/NOTICE && sleep 2 ) 1>&2 || true; \
	        fi; \
	      fi; \
	    ;; \
	    "FreeBSD"|"freebsd") \
	      if ! id $(RSERVRD_USER); then \
	        pw groupadd $(RSERVRD_GROUP) || true; \
	        if ! pw useradd $(RSERVRD_GROUP) -g $(RSERVRD_USER) -d $(RSERVRD_TABLE) -s /bin/false; then \
	          ( echo "{fail point 1[FreeBSD]}" && cat $(srcdir)/NOTICE && sleep 2 ) 1>&2 || true; \
	        fi; \
	      fi; \
	    ;; \
	  esac; \
	  if ! ( chown $(RSERVRD_USER):$(RSERVRD_GROUP) $(DESTDIR)$(bindir)/$(RSERVRD) &> /dev/null ) || \
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
	      ( [ -n "$(RSERVRD_DELETE_USER)" ] && [ "$(RSERVRD_DELETE_USER)" == 'true' ] && userdel $(RSERVRD_USER) &> /dev/null ) || true; \
	      ( [ -n "$(RSERVRD_DELETE_GROUP)" ] && [ "$(RSERVRD_DELETE_GROUP)" == 'true' ] && groupdel $(RSERVRD_GROUP) &> /dev/null ) || true; \
	    ;; \
	    "FreeBSD"|"freebsd") \
	      ( [ -n "$(RSERVRD_DELETE_USER)" ] && [ "$(RSERVRD_DELETE_USER)" == 'true' ] && pw userdel $(RSERVRD_USER) &> /dev/null ) || true; \
	      ( [ -n "$(RSERVRD_DELETE_GROUP)" ] && [ "$(RSERVRD_DELETE_GROUP)" == 'true' ] && pw groupdel $(RSERVRD_GROUP) &> /dev/null ) || true; \
	    ;; \
	  esac; \
	fi;
