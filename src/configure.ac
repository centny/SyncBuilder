# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([SyncBuilder], [1.0.0], [Centny@gmail.com])
#LT_INIT
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([SyncBuilder.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lcurl':
AX_LIB_CURL([7.00.0],,[AC_MSG_ERROR([Your system lacks of libcurl >= 7.00.0])])
AX_CHECK_LOG4CXX
AX_CHECK_EXTC
AX_BOOST_BASE([1.40],[],[AC_MSG_ERROR(Your system lacks of boost >= 1.40.0)])
AX_BOOST_DATE_TIME
AX_BOOST_FILESYSTEM
AX_BOOST_REGEX
AX_BOOST_SYSTEM
AX_BOOST_THREAD
AX_LIB_SQLITE3
# Checks for header files.
#AC_CHECK_HEADER([stdlib.h string.h curl.h log4cxx/logger.h])
#LDFLAGS=-lboost_system-mt
# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_OFF_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_CHECK_FUNCS([memset mkdir setlocale strchr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
