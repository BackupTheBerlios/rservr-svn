ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../../.. -I$(srcdir)/../../../include/rservr -I$(srcdir)/../../../include -I$(srcdir)/../../../libs/include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL $(ALL_INCLUDES)
bin_PROGRAMS=rsvbits
rsvbits_CPPFLAGS=$(ALL_INCLUDES) -DRSV_RELAY -DRSV_PIPE
rsvbits_SOURCES=rsvbits.cpp bit-data.cpp bit-data.hpp
rsvbits_LDADD=../../../libs/client/librservr-client.la
