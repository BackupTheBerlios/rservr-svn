import sys, os, traceback


file_load_order = [
    'symbols.py',
    'command.py',
    'client.py',
    'log-output.py',
    'admin-client.py',
    'service-client.py',
    'control-client.py',
    'resource-client.py',
    'monitor-client.py',
    'detached-client.py',
    'command-queue.py',
    'message-queue.py',
    'request.py',
    'response.py',
    'load-plugin.py',
    'command-table.py',
    'remote-service.py',
    'client-timing.py',
    'tools.py',
    'label-check.py',
    'rsvp-dataref-hook.py',
    'rsvp-dataref-thread.py',
    'rsvp-dataref.py',
    'rsvp-netcntl-hook.py',
    'rsvp-netcntl.py',
    'rsvp-passthru-assist.py',
    'rsvp-passthru-hook.py',
    'rsvp-passthru.py',
    'rsvp-ready-hook.py',
    'rsvp-ready.py',
    'rsvp-rqconfig-hook.py',
    'rsvp-rqconfig-thread.py',
    'rsvp-rqconfig.py',
    'rsvp-rqsrvc-auto.py',
    'rsvp-rqsrvc-hook.py',
    'rsvp-rqsrvc.py',
    'rsvp-trigger-hook.py',
    'rsvp-trigger.py',
    'config-parser.py'
]


def reload_scripts():
    for i in range(len(file_load_order)):
        filename = os.path.join(rservr.python_path, file_load_order[i])
        try:
            execfile(filename, globals(), vars(rservr))
        except Exception:
            traceback.print_exc(file=sys.stderr)
            print >> sys.stderr, 'failed to load', filename
            raise IOError


reload_scripts()

del file_load_order
del reload_scripts
