ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../../.. -I$(srcdir)/../../../include/rservr -I$(srcdir)/../../../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL $(ALL_INCLUDES)
bin_PROGRAMS=rsv-respawn

rsv_respawn_CPPFLAGS=$(ALL_INCLUDES)
rsv_respawn_SOURCES=rsv-respawn.c respawn-load.cpp messages.c respawn-load.h messages.h action-list.hpp
rsv_respawn_LDADD=../../../libs/client/librservr-client.la
