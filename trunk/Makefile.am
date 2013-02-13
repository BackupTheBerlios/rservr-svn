SUBDIRS=include lang libs plugins security programs
EXTRA_DIST=param.h attributes.h
export AM_CXXFLAGS=-Wall
export AM_CFLAGS=-Wall
export PLUGIN_PATH=$(libexecdir)/rservr
export STANDARD_INCLUDES=-I$(prefix)/include -I/usr/include -I/usr/local/include
export STANDARD_LIBRARIES=-L$(prefix)/lib -L/usr/lib -L/usr/local/lib -L$(PLUGIN_PATH)
export ACLOCAL_AMFLAGS=-I m4
