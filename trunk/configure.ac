#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(rservr-core, gamma.6, [Kevin P. Barry (ta0kira@users.berlios.de)])
AM_INIT_AUTOMAKE([dist-bzip2])
PACKAGE=rservr-core
VERSION=gamma.6

AC_CONFIG_FILES([ Makefile
                  include/Makefile
                  lang/Makefile
                  lang/us-english/Makefile
                  libs/Makefile
                  libs/protocol/Makefile
                  libs/command/Makefile
                  libs/proto/Makefile
                  libs/proto/proto-server/Makefile
                  libs/proto/proto-common/Makefile
                  libs/proto/proto-admin/Makefile
                  libs/proto/proto-service/Makefile
                  libs/proto/proto-control/Makefile
                  libs/proto/proto-resource/Makefile
                  libs/proto/proto-monitor/Makefile
                  libs/server/Makefile
                  libs/client/Makefile
                  libs/filter/Makefile
                  plugins/Makefile
                  plugins/netcntl/Makefile
                  plugins/rqsrvc/Makefile
                  plugins/rqconfig/Makefile
                  plugins/ready/Makefile
                  plugins/dataref/Makefile
                  security/Makefile
                  security/rsvx-localhost/Makefile
                  security/rsvx-ipregex/Makefile
                  security/rsvx-dnsregex/Makefile
                  security/rsvx-penalty/Makefile
                  security/rsvx-tls/Makefile
                  security/rsvx-inert/Makefile
                  programs/Makefile
                  programs/server/Makefile
                  programs/server/rservr/Makefile
                  programs/clients/Makefile
                  programs/clients/rservrd/Makefile
                  programs/clients/rsv-forward/Makefile
                  programs/clients/rsv-echo/Makefile
                  programs/clients/rsv-terminal/Makefile
                  programs/clients/rsv-respawn/Makefile
                  programs/filters/Makefile
                  programs/filters/rsvf-log/Makefile
                  programs/other/Makefile
                  programs/other/safexec/Makefile
                  programs/other/rsvbits/Makefile
                  programs/other/pipexec/Makefile
                  programs/other/unsterm/Makefile ])

AC_CONFIG_SRCDIR([param.h])

# Checks for programs.
AC_GNU_SOURCE
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AC_PROG_MKDIR_P

export CPPFLAGS='-I/usr/include -I/usr/local/include'
export LDFLAGS='-L/usr/lib -L/usr/local/lib'

# Checks for libraries.
AC_CHECK_LIB([dl], [dlopen], [LIBDL=true], [], [])
AC_CHECK_LIB([pthread], [pthread_create], [LIBPTHREAD=true], [], [])

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_HEADER_STAT
AC_HEADER_TIME
AC_CHECK_HEADERS_ONCE([arpa/inet.h dlfcn.h errno.h fcntl.h grp.h libgen.h math.h netdb.h netinet/in.h pthread.h pwd.h regex.h signal.h stddef.h stdint.h stdio.h stdlib.h string.h sys/ipc.h sys/select.h sys/socket.h sys/stat.h sys/types.h sys/wait.h termios.h time.h unistd.h gcrypt.h], [], [AC_MSG_ERROR([system header missing!])])
AC_CHECK_HEADERS_ONCE([hparser/formats/descriptor-output.hpp hparser/formats/tagged-input.hpp hparser/impl-defined.hpp hparser/linked-section.hpp hparser/multi-destination.hpp hparser/multi-source.hpp hparser/output-manager.hpp hparser/structure-base.hpp], [], [AC_MSG_ERROR(['hparser' header missing!])], [])
AC_CHECK_HEADERS([readline/readline.h], [READLINE=true], [], []) #not _ONCE!
AC_CHECK_HEADERS([readline/history.h], [HISTORY=true], [], []) #not _ONCE!
AC_CHECK_HEADERS([gnutls/gnutls.h], [GNUTLS=true], [], []) #not _ONCE!
AC_CHECK_HEADERS([gnutls/extra.h], [EXTRA=true], [], []) #not _ONCE!

# Set custom build flags.
AM_CONDITIONAL([LINK_READLINE], [test x$READLINE = xtrue])
AM_CONDITIONAL([LINK_HISTORY], [test x$HISTORY = xtrue])
AM_CONDITIONAL([LINK_GNUTLS], [test x$GNUTLS = xtrue && test x$EXTRA = xtrue])
AM_CONDITIONAL([LINK_LIBDL], [test x$LIBDL = xtrue])
AM_CONDITIONAL([LINK_LIBPTHREAD], [test x$LIBPTHREAD = xtrue])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_INT8_T
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_HEADER_TIME
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_C_RESTRICT
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_CHOWN
AC_FUNC_FORK
AC_FUNC_GETPGRP
AC_FUNC_SETPGRP
AC_FUNC_MALLOC
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_SETVBUF_REVERSED
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_STRTOD
AC_CHECK_FUNCS([dup2 ftruncate getcwd inet_ntoa mkdir regcomp select socket strdup strerror strtoul endgrent readline putenv setenv strtol strtoull clearenv])

#other flags
m4_pattern_allow(AM_PROG_CC_C_O)
AM_PROG_CC_C_O
AC_CONFIG_MACRO_DIR([m4])

AC_OUTPUT
