# Copyright (C) 2007 the authors.
#
# This file is part of the Fhtagn! C++ Library, and may be distributed under
# the following license terms and conditions as set forth in the COPYING file
# included in this source distribution.

SUBDIRS = text restrictions detail
if HAVE_BOOST
SUBDIRS += xml threads
endif
# currently, all the util lib contains is a pretty-printer for cppunit
# test results
if HAVE_CPPUNIT
SUBDIRS += util
endif

include $(top_srcdir)/Makefile.common

lib_LTLIBRARIES = libfhtagn.la

FHTAGN_SRC= version.cpp
FHTAGN_HDR= \
	version.h \
	pointers.h property.h mandatory.h \
	restricted.h restrictions.h \
	byteorder.h

if HAVE_BOOST
FHTAGN_SRC += variant.cpp
FHTAGN_HDR += variant.h stdvariant.h detail/variant.tcc
endif

libfhtagn_la_SOURCES = $(FHTAGN_SRC)

libfhtagn_la_LDFLAGS = $(LIB_VERSION)
libfhtagn_la_LIBADD = \
	$(top_builddir)/fhtagn/text/libfhtagn_text.la \
	$(top_builddir)/fhtagn/threads/libfhtagn_threads.la

pkginclude_HEADERS = config.h $(FHTAGN_HDR)
