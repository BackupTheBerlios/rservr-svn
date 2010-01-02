ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../.. -I$(srcdir)/../../include/rservr -I$(srcdir)/../../include -I$(srcdir)/../include
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL -DRSERVR_RESTRICT_COMMAND_INTERFACE $(ALL_INCLUDES)
lib_LTLIBRARIES=librsvp-rqsrvc.la librsvp-rqsrvc-auto.la
librsvp_rqsrvc_la_SOURCES=api-rqsrvc.cpp plugin-rqsrvc.cpp api-rqsrvc.h plugin-rqsrvc.hpp
librsvp_rqsrvc_la_LIBADD=../../libs/command/librservr-command.la
librsvp_rqsrvc_auto_la_SOURCES=rqsrvc-auto.cpp
librsvp_rqsrvc_auto_la_LIBADD=librsvp-rqsrvc.la ../../libs/client/librservr-client.la
