# Copyright (C) 2007 the authors.
#
# This file is part of the Fhtagn! C++ Library, and may be distributed under
# the following license terms and conditions as set forth in the COPYING file
# included in this source distribution.

include $(top_srcdir)/Makefile.common
utilincludedir=$(pkgincludedir)/util

lib_LTLIBRARIES = libfhtagn_util.la

UTIL_SRC=
UTIL_HDR=

if HAVE_CPPUNIT
UTIL_SRC += cppunit_output.cpp
UTIL_HDR += cppunit_output.h
endif

libfhtagn_util_la_SOURCES = $(UTIL_SRC)

libfhtagn_util_la_LDFLAGS = $(LIB_VERSION) $(LDFLAGS_FHTAGN)

utilinclude_HEADERS = $(UTIL_HDR)
