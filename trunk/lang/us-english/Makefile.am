AM_LDFLAGS=-version-info 3:0:3
ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../.. -I$(srcdir)/../../include/rservr -I$(srcdir)/.. -I$(srcdir)/../../libs/include/protocol
AM_CPPFLAGS=-DENABLE_VIRT_ARRAY_ALL $(ALL_INCLUDES)
lib_LTLIBRARIES=librservr-i18n-us-english.la
librservr_i18n_us_english_la_SOURCES=client.c command.c general.c proto.c protocol.c server.c command-info.c
librservr_i18n_us_english_la_LIBADD=
EXTRA_DIST=../translation.h
