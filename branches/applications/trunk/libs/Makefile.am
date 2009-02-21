SUBDIRS=protocol command proto server client filter
EXTRA_DIST=include/module-macro.hpp

install-exec-hook:
	if [ "$$(id -u 2> /dev/null)" -eq 0 ]; then \
	  /sbin/ldconfig $(DESTDIR)$(libdir); \
	fi;
