command_reference = long
text_info = str
info_list = list
binary_info = str
binary_size = long
command_text = str
command_priority = long
result = bool
multi_result = long
entity_type = long
permission_mask = long
security_level = long
response_type = long
server_directive = long
command_type = long
command_event = long
client_time = long
long_time = float
short_time = float
logging_mode = long
create_flags = long
io_device = long
monitor_event = long


class command_handle(long):
     def __new__(cls, value):
         return long.__new__(cls, value)

class entity_handle(long):
     def __new__(cls, value):
         return long.__new__(cls, value)

class message_handle(long):
     def __new__(cls, value):
         return long.__new__(cls, value)
