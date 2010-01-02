if LINK_LIBPTHREAD
  USE_LIBPTHREAD=-lpthread
endif

ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../../.. -I$(srcdir)/../../../include/rservr -I$(srcdir)/../../../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL $(ALL_INCLUDES)
bin_PROGRAMS=rsv-relay rsv-controller rsv-netrelay rsv-netcontroller rsv-fsrelay rsv-fscontroller

rsv_relay_CPPFLAGS=$(ALL_INCLUDES) -DRSV_RELAY -DRSV_PIPE
rsv_relay_SOURCES=rsv-forward.c messages.c monitor-services.cpp static-auto-mutex.cpp messages.h monitor-services.h static-auto-mutex.h
rsv_relay_LDADD=$(STANDARD_LIBRARIES) $(USE_LIBPTHREAD) ../../../plugins/rqsrvc/librsvp-rqsrvc-auto.la ../../../libs/client/librservr-client.la

rsv_controller_CPPFLAGS=$(ALL_INCLUDES) -DRSV_CONTROLLER -DRSV_PIPE
rsv_controller_SOURCES=rsv-forward.c static-auto-mutex.cpp static-auto-mutex.h
rsv_controller_LDADD=$(STANDARD_LIBRARIES) $(USE_LIBPTHREAD) ../../../libs/client/librservr-client.la

rsv_netrelay_CPPFLAGS=$(ALL_INCLUDES) -DRSV_RELAY -DRSV_NET -DRSV_SOCKET
rsv_netrelay_SOURCES=rsv-forward.c socket-table.cpp socket-check.c messages.c monitor-services.cpp security-filter.cpp remote-filter.c select-breaker.c listen.cpp connect.cpp static-auto-mutex.cpp socket-table.h socket-check.h messages.h monitor-services.h security-filter.h remote-filter.h listen.hpp connect.hpp select-breaker.h static-auto-mutex.h
rsv_netrelay_LDADD=$(STANDARD_LIBRARIES) $(USE_LIBPTHREAD) ../../../plugins/rqsrvc/librsvp-rqsrvc-auto.la ../../../plugins/netcntl/librsvp-netcntl.la ../../../libs/client/librservr-client.la

rsv_netcontroller_CPPFLAGS=$(ALL_INCLUDES) -DRSV_CONTROLLER -DRSV_NET -DRSV_SOCKET
rsv_netcontroller_SOURCES=rsv-forward.c socket-table.cpp messages.c security-filter.cpp socket-check.c remote-filter.c select-breaker.c listen.cpp connect.cpp static-auto-mutex.cpp socket-table.h socket-check.h messages.h security-filter.h remote-filter.h listen.hpp connect.hpp select-breaker.h static-auto-mutex.h
rsv_netcontroller_LDADD=$(STANDARD_LIBRARIES) $(USE_LIBPTHREAD) ../../../plugins/netcntl/librsvp-netcntl.la ../../../libs/client/librservr-client.la

rsv_fsrelay_CPPFLAGS=$(ALL_INCLUDES) -DRSV_RELAY -DRSV_LOCAL -DRSV_SOCKET
rsv_fsrelay_SOURCES=rsv-forward.c socket-table.cpp socket-check.c messages.c monitor-services.cpp security-filter.cpp remote-filter.c select-breaker.c listen.cpp connect.cpp static-auto-mutex.cpp socket-table.h socket-check.h messages.h monitor-services.h security-filter.h remote-filter.h listen.hpp connect.hpp select-breaker.h static-auto-mutex.h
rsv_fsrelay_LDADD=$(STANDARD_LIBRARIES) $(USE_LIBPTHREAD) ../../../plugins/rqsrvc/librsvp-rqsrvc-auto.la ../../../plugins/netcntl/librsvp-netcntl.la ../../../libs/client/librservr-client.la

rsv_fscontroller_CPPFLAGS=$(ALL_INCLUDES) -DRSV_CONTROLLER -DRSV_LOCAL -DRSV_SOCKET
rsv_fscontroller_SOURCES=rsv-forward.c socket-table.cpp socket-check.c messages.c security-filter.cpp remote-filter.c select-breaker.c listen.cpp connect.cpp static-auto-mutex.cpp socket-table.h socket-check.h messages.h security-filter.h remote-filter.h listen.hpp connect.hpp select-breaker.h static-auto-mutex.h
rsv_fscontroller_LDADD=$(STANDARD_LIBRARIES) $(USE_LIBPTHREAD) ../../../plugins/netcntl/librsvp-netcntl.la ../../../libs/client/librservr-client.la
