#!/usr/bin/python

import sys


if len(sys.argv) != 2 or not len(sys.argv[1]):
     print >> sys.stderr, '%s [client name]' % (sys.argv[0])
     quit(1)


import rservr, rservr.rsvp_rqsrvc


def queue_event_hook(event):
    if event == rservr.RSERVR_QUEUE_MESSAGE:
        while True:
            try:
                message = rservr.current_message()
            except IndexError:
                break
            try:
                if message.is_info():
                    print >> sys.stderr, 'info message: "%s"' % (message.to_info_message())
                    response = rservr.short_response(message, rservr.event_complete)
                    rservr.send_command_no_status(response)
                    rservr.destroy_command(response)
                elif message.is_request():
                    print >> sys.stderr, 'request message: "%s"' % (message.to_request_message())
                    response = rservr.client_response(message, rservr.event_complete, message.to_request_message())
                    rservr.send_command_no_status(response)
                    rservr.destroy_command(response)
                rservr.remove_message(message)
            except (TypeError, IndexError, RuntimeError):
                rservr.remove_message(message)


def __rsvp_rqsrvc_auto_hook_type_check(type_string, name_string):
    return type_string == 'echo'

rservr.rsvp_rqsrvc.__rsvp_rqsrvc_auto_hook_type_check = __rsvp_rqsrvc_auto_hook_type_check


print >> sys.stderr, 'initializing...'
rservr.initialize_client()

print >> sys.stderr, 'starting message queue...'
rservr.start_message_queue()

print >> sys.stderr, 'registering client...'
rservr.register_resource_client()

print >> sys.stderr, 'registering service...'
service = rservr.register_service(sys.argv[1], 'echo')
rservr.send_command_no_status(service)
rservr.destroy_command(service)


print >> sys.stderr, 'inlining message queue...'
rservr.stop_message_queue()
rservr.set_queue_event_hook(queue_event_hook)

try:
    outcome = rservr.inline_message_queue()
except RuntimeError:
    outcome = False

print >> sys.stderr, 'cleaning up...'
rservr.client_cleanup()

print >> sys.stderr, 'done.'
quit(0 if outcome else 1)
