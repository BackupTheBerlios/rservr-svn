STANDARD_INCLUDES=-I/usr/include -I/usr/local/include
ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../../..
AM_CPPFLAGS=$(ALL_INCLUDES)
AM_CXXFLAGS=-ansi -Wall
AM_CFLAGS=-Wall
AM_LDFLAGS=
bin_PROGRAMS=unsterm

if LINK_READLINE
  USE_READLINE=-lreadline -lncurses
endif

if LINK_HISTORY
  USE_HISTORY=-lhistory
endif

unsterm_SOURCES=unsterm.c
unsterm_LDADD=$(USE_READLINE) $(USE_HISTORY)
