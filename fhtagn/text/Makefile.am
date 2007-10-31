# Copyright (C) 2007 the authors.
#
# This file is part of the Fhtagn! C++ Library, and may be distributed under
# the following license terms and conditions as set forth in the COPYING file
# included in this source distribution.

SUBDIRS = detail

include $(top_srcdir)/Makefile.common
textincludedir=$(pkgincludedir)/text

noinst_LTLIBRARIES = libfhtagn_text.la

libfhtagn_text_la_SOURCES = \
	transcoding.cpp \
	decoders.cpp \
	encoders.cpp

libfhtagn_text_la_LDFLAGS = $(LIB_VERSION)

textinclude_HEADERS = \
	transcoding.h decoders.h encoders.h xparser.h
