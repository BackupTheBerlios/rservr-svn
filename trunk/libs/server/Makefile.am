if LINK_LIBPTHREAD
  USE_LIBPTHREAD=-lpthread
endif

ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/local-include -I$(srcdir)/../.. -I$(srcdir)/../../include/rservr -I$(srcdir)/../../include -I$(srcdir)/../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL $(ALL_INCLUDES)
lib_LTLIBRARIES=librservr-server.la
librservr_server_la_SOURCES=api-server.cpp client-thread.cpp execute-thread.cpp monitor-list.cpp server-timing.cpp client-list.cpp default-server-timing.c local-client.cpp server-command.cpp server-signal.cpp service-list.cpp name-check.cpp monitors.cpp execute-module.cpp server-load.c
librservr_server_la_LIBADD=$(USE_LIBPTHREAD) ../proto/proto-server/librservr-proto-server.la ../proto/proto-common/librservr-proto-common.la ../proto/proto-admin/librservr-proto-admin.la ../proto/proto-service/librservr-proto-service.la ../proto/proto-control/librservr-proto-control.la ../proto/proto-resource/librservr-proto-resource.la ../proto/proto-monitor/librservr-proto-monitor.la ../protocol/librservr-protocol.la ../../lang/us-english/librservr-i18n-us-english.la
EXTRA_DIST=local-include/api-server.hpp local-include/api-server-interface.hpp local-include/client-list.hpp local-include/client-thread.hpp local-include/execute-thread.hpp local-include/local-client.hpp local-include/monitor-list.hpp local-include/server-command.hpp local-include/server-parameters.h local-include/service-list.hpp local-include/server-signal.h local-include/server-timing.hpp local-include/name-check.hpp local-include/monitors.h local-include/monitor-update.hpp local-include/execute-queue.hpp local-include/execute-module.hpp local-include/priority-adjust.hpp local-include/protected-server.hpp local-include/server-load.h ../include/server/server.h
