# Copyright (C) 2007 the authors.
#
# This file is part of the Fhtagn! C++ Library, and may be distributed under
# the following license terms and conditions as set forth in the COPYING file
# included in this source distribution.
#
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.58)
AC_COPYRIGHT([Copyright (C) 2007 the authors. - see COPYING for licensing terms])
AC_REVISION([$Revision$])

AC_INIT(fhtagn, 0.1, fhtagn-devel@lists.sourceforge.net)
AC_CONFIG_SRCDIR([fhtagn/variant.cpp])
AM_INIT_AUTOMAKE

AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
AX_BOOST_BASE([1.33.1])
AM_PATH_CPPUNIT

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_UINT32_T

# Checks for library functions.

# Optional features
AM_FHTAGN_ENABLE_EXTRA_CHECKS
AM_FHTAGN_ENABLE_COVERAGE
AM_FHTAGN_ENABLE_DEBUG

AC_CONFIG_FILES([
  Makefile
  fhtagn/Makefile
  fhtagn/detail/Makefile
  fhtagn/restrictions/Makefile
  fhtagn/util/Makefile
  fhtagn/text/Makefile
  fhtagn/xml/Makefile
  test/Makefile
])
AC_OUTPUT
