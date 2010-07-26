if LINK_LIBDL
  USE_LIBDL=-ldl
endif

if LINK_LIBPTHREAD
  USE_LIBPTHREAD=-lpthread
endif

AM_LDFLAGS=-version-info 3:0:3
ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/local-include -I$(srcdir)/../.. -I$(srcdir)/../../include/rservr -I$(srcdir)/../../include -I$(srcdir)/../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL -DPLUGIN_PATH=\"$(PLUGIN_PATH)\" $(ALL_INCLUDES)
lib_LTLIBRARIES=librservr-client.la
librservr_client_la_SOURCES=api-admin-client.cpp api-control-client.cpp api-monitor-client.cpp api-response.cpp client-output.cpp api-client-timing.cpp api-load-plugin.cpp api-message-queue.cpp api-service-client.cpp default-client-timing.c api-client.cpp api-request.cpp client-command.cpp api-command-queue.cpp api-resource-client.cpp client-input.cpp api-remote-service.cpp command-queue.cpp
librservr_client_la_LIBADD=$(USE_LIBDL) $(USE_LIBPTHREAD) ../proto/proto-server/librservr-proto-server.la ../proto/proto-common/librservr-proto-common.la ../proto/proto-admin/librservr-proto-admin.la ../proto/proto-service/librservr-proto-service.la ../proto/proto-control/librservr-proto-control.la ../proto/proto-resource/librservr-proto-resource.la ../proto/proto-monitor/librservr-proto-monitor.la ../protocol/librservr-protocol.la ../../lang/us-english/librservr-i18n-us-english.la
EXTRA_DIST=local-include/api-admin-client.hpp local-include/api-command-queue.hpp local-include/api-message-queue.hpp local-include/api-response.hpp local-include/client-input.hpp local-include/api-client-timing.hpp local-include/api-control-client.hpp local-include/api-request.hpp local-include/api-service-client.hpp local-include/client-output.hpp local-include/api-client.hpp local-include/api-load-plugin.hpp local-include/api-monitor-client.hpp local-include/api-resource-client.hpp local-include/client-command.hpp local-include/command-macro.hpp local-include/client-response-macro.hpp local-include/api-remote-service.hpp local-include/command-queue.hpp
