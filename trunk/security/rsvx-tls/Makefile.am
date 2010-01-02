ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../.. -I$(srcdir)/../../include/rservr -I$(srcdir)/../../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL $(ALL_INCLUDES)
lib_LTLIBRARIES=librsvx-tls.la
librsvx_tls_la_SOURCES=rsvx-tls.cpp
librsvx_tls_la_LIBADD=-lgnutls -lgnutls-extra -lgcrypt
