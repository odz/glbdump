#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([glbdump], [0.01], [oda.org@gmail.com])
AC_CONFIG_AUX_DIR([libltdl/config])
AM_INIT_AUTOMAKE([0.01 dist-bzip2])
AC_CONFIG_SRCDIR([glbdump.cc])
AC_CONFIG_HEADERS([config.h])

LT_PREREQ([2.4])
LT_INIT

LT_CONFIG_LTDL_DIR([libltdl])
AC_CONFIG_MACRO_DIR([libltdl/m4])
LTDL_INIT

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.
AC_CHECK_FUNCS([realpath])

AC_OUTPUT([Makefile])
