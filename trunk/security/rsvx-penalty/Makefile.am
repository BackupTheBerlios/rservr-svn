if LINK_LIBPTHREAD
  USE_LIBPTHREAD=-lpthread
endif

ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../.. -I$(srcdir)/../../include/rservr -I$(srcdir)/../../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL $(ALL_INCLUDES)
lib_LTLIBRARIES=librsvx-penalty.la
librsvx_penalty_la_SOURCES=rsvx-penalty.cpp
librsvx_penalty_la_LIBADD=$(USE_LIBPTHREAD)
