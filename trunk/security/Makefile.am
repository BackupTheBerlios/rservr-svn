if LINK_GNUTLS
  USE_GNUTLS=rsvx-tls
endif

SUBDIRS=rsvx-localhost rsvx-ipregex rsvx-dnsregex rsvx-penalty $(USE_GNUTLS) rsvx-inert

install-exec-hook:
	if [ "$$(id -u 2> /dev/null)" -eq 0 ]; then \
	  /sbin/ldconfig $(DESTDIR)$(libdir); \
	fi;
