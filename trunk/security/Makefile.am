SUBDIRS=rsvx-localhost rsvx-ipregex rsvx-dnsregex rsvx-penalty rsvx-inert rsvx-testing

install-exec-hook:
	if [ "$$(id -u 2> /dev/null)" -eq 0 ]; then \
	  /sbin/ldconfig $(DESTDIR)$(libdir); \
	fi;
