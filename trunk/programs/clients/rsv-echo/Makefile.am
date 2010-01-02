ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../../.. -I$(srcdir)/../../../include/rservr
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL $(ALL_INCLUDES)
bin_PROGRAMS=rsv-echo
rsv_echo_SOURCES=rsv-echo.c
rsv_echo_LDADD=../../../plugins/rqsrvc/librsvp-rqsrvc-auto.la ../../../libs/client/librservr-client.la
