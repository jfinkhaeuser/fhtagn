# Copyright (C) 2007 the authors.
#
# This file is part of the Fhtagn! C++ Library, and may be distributed under
# the following license terms and conditions as set forth in the COPYING file
# included in this source distribution.

SUBDIRS = util xml

include $(top_srcdir)/Makefile.common

lib_LTLIBRARIES = libfhtagn.la

libfhtagn_la_SOURCES = \
	variant.cpp

libfhtagn_la_LDFLAGS = $(LIB_VERSION)

pkginclude_HEADERS = \
	variant.h variant.tcc stdvariant.h \
	pointers.h
