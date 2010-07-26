AM_LDFLAGS=-version-info 3:0:3
ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../.. -I$(srcdir)/../../include/rservr -I$(srcdir)/../../include -I$(srcdir)/../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL $(ALL_INCLUDES)
lib_LTLIBRARIES=librservr-filter.la
librservr_filter_la_SOURCES=filter-main.c api-filter.c api-filter.h
librservr_filter_la_LIBADD=
