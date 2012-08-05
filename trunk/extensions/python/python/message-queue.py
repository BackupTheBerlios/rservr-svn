message_dimension = long


class message_info(long):
     def __new__(cls, value):
         return long.__new__(cls, value)
     def __getattr__(self, name):
         return rservr._message_info_getattr(self, name)

class incoming_request_data(long):
     def __new__(cls, value):
         return long.__new__(cls, value)
     def __getattr__(self, name):
         return rservr._incoming_request_data_getattr(self, name)

class incoming_response_data(long):
     def __new__(cls, value):
         return long.__new__(cls, value)
     def __getattr__(self, name):
         return rservr._incoming_response_data_getattr(self, name)

class incoming_remote_data(long):
     def __new__(cls, value):
         return long.__new__(cls, value)
     def __getattr__(self, name):
         return rservr._incoming_remote_data_getattr(self, name)

class incoming_info_data(long):
     def __new__(cls, value):
         return long.__new__(cls, value)
     def __getattr__(self, name):
         return rservr._incoming_info_data_getattr(self, name)
