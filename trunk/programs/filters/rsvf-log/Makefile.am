ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../../.. -I$(srcdir)/../../../include/rservr
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL $(ALL_INCLUDES)
bin_PROGRAMS=rsvf-log
rsvf_log_SOURCES=rsvf-log.c
rsvf_log_LDADD= ../../../libs/filter/librservr-filter.la
