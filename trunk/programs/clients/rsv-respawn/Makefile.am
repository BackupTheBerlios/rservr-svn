STANDARD_INCLUDES=-I/usr/include -I/usr/local/include
ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../../.. -I$(srcdir)/../../../include/rservr -I$(srcdir)/../../../include
AM_CPPFLAGS=$(ALL_INCLUDES)
AM_CXXFLAGS=-DENABLE_VIRT_ARRAY_ALL -ansi -Wall
AM_CFLAGS=-Wall
AM_LDFLAGS=
bin_PROGRAMS=rsv-respawn

rsv_respawn_CPPFLAGS=$(ALL_INCLUDES)
rsv_respawn_SOURCES=rsv-respawn.c respawn-load.cpp messages.c respawn-load.h messages.h action-list.hpp
rsv_respawn_LDADD=../../../libs/client/librservr-client.la
