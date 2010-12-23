SUBDIRS=rsvp-netcntl rsvp-rqsrvc rsvp-rqconfig rsvp-ready rsvp-dataref rsvp-trigger
EXTRA_DIST=include/api-macro.h include/plugin-macro.hpp

install-exec-hook:
	if [ "$$(id -u 2> /dev/null)" -eq 0 ]; then \
	  /sbin/ldconfig $(DESTDIR)$(libdir); \
	fi;
