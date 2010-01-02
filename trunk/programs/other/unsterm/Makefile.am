ALL_INCLUDES=$(STANDARD_INCLUDES) -I$(srcdir)/../../..
AM_CPPFLAGS=$(ALL_INCLUDES)
bin_PROGRAMS=unsterm

if LINK_READLINE
  USE_READLINE=-lreadline -lncurses
endif

if LINK_HISTORY
  USE_HISTORY=-lhistory
endif

unsterm_SOURCES=unsterm.c
unsterm_LDADD=$(STANDARD_LIBRARIES) $(USE_READLINE) $(USE_HISTORY)
