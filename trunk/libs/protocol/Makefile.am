AM_LDFLAGS=-version-info 3:0:3
ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/local-include -I$(srcdir)/../.. -I$(srcdir)/../../include/rservr -I$(srcdir)/../../include -I$(srcdir)/../include/protocol -I$(srcdir)/../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL $(ALL_INCLUDES)
lib_LTLIBRARIES=librservr-protocol.la
librservr_protocol_la_SOURCES=api-config-parser.cpp api-timing.cpp cstring-input.cpp api-tools.cpp common-input.cpp common-output.cpp timing-manager.c timing-query.cpp constants.cpp conversion.cpp
librservr_protocol_la_LIBADD=$(STANDARD_LIBRARIES) -lhparser -lhpaux
EXTRA_DIST=../include/protocol/common-output.hpp ../include/protocol/local-check.h ../include/protocol/logging.h ../include/protocol/server-timing.h ../include/protocol/api-timing.h ../include/protocol/cstring-input.hpp ../include/protocol/local-timing.h ../include/protocol/monitor-standby.hpp ../include/protocol/retry-limit.hpp ../include/protocol/api-tools.hpp ../include/protocol/common-input.hpp ../include/protocol/local-types.h ../include/protocol/server-interface.hpp ../include/protocol/timing-manager.h ../include/protocol/timing-query.hpp ../include/protocol/constants.hpp ../include/protocol/conversion.hpp ../include/protocol/client-descriptors.h
