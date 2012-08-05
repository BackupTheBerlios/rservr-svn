#!/usr/bin/env rsv-python

rservr.initialize_client()
rservr.start_message_queue()

rservr.register_resource_client()
service = rservr.register_service(rservr.argv[0] if rservr.argc >= 1 else '', 'test')
rservr.send_command_no_status(service)
rservr.destroy_command(service)

while True:
    try:
        rservr.message_queue_sync()
    except RuntimeError:
        quit()
    try:
        message = rservr.current_message()
    except ValueError:
        pass
    try:
        if message.is_info:
            print 'info message: "%s"' % (message.to_info_message)
            response = rservr.short_response(message, rservr.event_complete)
            rservr.send_command_no_status(response)
            rservr.destroy_command(response)
        elif message.is_request:
            print 'request message: "%s"' % (message.to_request_message)
            response = rservr.client_response(message, rservr.event_complete, message.to_request_message)
            rservr.send_command_no_status(response)
            rservr.destroy_command(response)
        rservr.remove_message(message)
    except ValueError:
        pass

rservr.deregister_client()
rservr.stop_message_queue()
rservr.client_cleanup()
