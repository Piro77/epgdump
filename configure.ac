#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([epgdump], [0.1.0], [piro77@gmail.com])
AM_INIT_AUTOMAKE([foreign])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CONFIG_SRCDIR([nit.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h sys/time.h unistd.h])

case "${host_os}" in
    freebsd*)
     CFLAGS_save="${CFLAGS} -I/usr/local/include"
     CFLAGS="${CFLAGS_save}"
     LDFLAGS_save="${LDFLAGS} -L/usr/local/lib"
     LDFLAGS="${LDFLAGS_save}"
     ;;
esac
AM_ICONV
# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MKTIME
AC_CHECK_FUNCS([gettimeofday memmove memset strstr strdup])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
