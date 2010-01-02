ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../../.. -I$(srcdir)/../../../include/rservr -I$(srcdir)/../../../libs/include/server
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL $(ALL_INCLUDES)
bin_PROGRAMS=rservr
rservr_SOURCES=rservr.c
rservr_LDADD=../../../libs/server/librservr-server.la
