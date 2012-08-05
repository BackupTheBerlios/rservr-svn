AM_LDFLAGS=-version-info 1:0:0
ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../.. -I$(srcdir)/../../include/rservr -I$(srcdir)/src
AM_CPPFLAGS=$(ALL_INCLUDES)

pythondir=$(PLUGIN_PATH)/python
PYTHON_LOAD=\"execfile('$(pythondir)/load-all.py')\"

lib_LTLIBRARIES=librsve-python.la

librsve_python_la_SOURCES=
librsve_python_la_CPPFLAGS=$(AM_CPPFLAGS) $(PYTHON_CFLAGS)
librsve_python_la_LIBADD=$(PYTHON_LDFLAGS) ../../libs/client/librservr-client.la ../../plugins/rsvp-dataref/librsvp-dataref.la ../../plugins/rsvp-netcntl/librsvp-netcntl.la ../../plugins/rsvp-passthru/librsvp-passthru.la ../../plugins/rsvp-passthru/librsvp-passthru-assist.la ../../plugins/rsvp-ready/librsvp-ready.la ../../plugins/rsvp-rqconfig/librsvp-rqconfig.la ../../plugins/rsvp-rqsrvc/librsvp-rqsrvc.la ../../plugins/rsvp-rqsrvc/librsvp-rqsrvc-auto.la ../../plugins/rsvp-trigger/librsvp-trigger.la

bin_PROGRAMS=rsv-python

rsv_python_CPPFLAGS=$(AM_CPPFLAGS) -I$(srcdir)/../../include/rservr/api -I$(srcdir)/../../include/rservr/plugins $(PYTHON_CFLAGS) "-DPYTHON_LOAD=$(PYTHON_LOAD)" "-DPYTHON_PATH=\"$(pythondir)\""
rsv_python_SOURCES=rsv-python.c src/load-all.c src/command.c src/client.c src/log-output.c src/admin-client.c src/service-client.c src/control-client.c src/resource-client.c src/monitor-client.c src/detached-client.c src/command-queue.c src/message-queue.c src/request.c src/response.c src/load-plugin.c src/command-table.c src/remote-service.c src/client-timing.c src/tools.c src/label-check.c src/rsvp-dataref-hook.c src/rsvp-dataref-thread.c src/rsvp-dataref.c src/rsvp-netcntl-hook.c src/rsvp-netcntl.c src/rsvp-passthru-assist.c src/rsvp-passthru-hook.c src/rsvp-passthru.c src/rsvp-ready-hook.c src/rsvp-ready.c src/rsvp-rqconfig-hook.c src/rsvp-rqconfig-thread.c src/rsvp-rqconfig.c src/rsvp-rqsrvc-auto.c src/rsvp-rqsrvc-hook.c src/rsvp-rqsrvc.c src/rsvp-trigger-hook.c src/rsvp-trigger.c src/config-parser.c
rsv_python_LDADD=librsve-python.la
EXTRA_DIST=src/load-all.h src/python-macro.h

python_HEADERS=$(srcdir)/python/load-all.py $(srcdir)/python/symbols.py $(srcdir)/python/command.py $(srcdir)/python/client.py $(srcdir)/python/log-output.py $(srcdir)/python/admin-client.py $(srcdir)/python/service-client.py $(srcdir)/python/control-client.py $(srcdir)/python/resource-client.py $(srcdir)/python/monitor-client.py $(srcdir)/python/detached-client.py $(srcdir)/python/command-queue.py $(srcdir)/python/message-queue.py $(srcdir)/python/request.py $(srcdir)/python/response.py $(srcdir)/python/load-plugin.py $(srcdir)/python/command-table.py $(srcdir)/python/remote-service.py $(srcdir)/python/client-timing.py $(srcdir)/python/tools.py $(srcdir)/python/label-check.py $(srcdir)/python/rsvp-dataref-hook.py $(srcdir)/python/rsvp-dataref-thread.py $(srcdir)/python/rsvp-dataref.py $(srcdir)/python/rsvp-netcntl-hook.py $(srcdir)/python/rsvp-netcntl.py $(srcdir)/python/rsvp-passthru-assist.py $(srcdir)/python/rsvp-passthru-hook.py $(srcdir)/python/rsvp-passthru.py $(srcdir)/python/rsvp-ready-hook.py $(srcdir)/python/rsvp-ready.py $(srcdir)/python/rsvp-rqconfig-hook.py $(srcdir)/python/rsvp-rqconfig-thread.py $(srcdir)/python/rsvp-rqconfig.py $(srcdir)/python/rsvp-rqsrvc-auto.py $(srcdir)/python/rsvp-rqsrvc-hook.py $(srcdir)/python/rsvp-rqsrvc.py $(srcdir)/python/rsvp-trigger-hook.py $(srcdir)/python/rsvp-trigger.py $(srcdir)/python/config-parser.py
