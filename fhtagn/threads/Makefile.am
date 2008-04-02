# Copyright (C) 2008 the authors.
#
# This file is part of the Fhtagn! C++ Library, and may be distributed under
# the following license terms and conditions as set forth in the COPYING file
# included in this source distribution.

#SUBDIRS = 

include $(top_srcdir)/Makefile.common
threadsincludedir=$(pkgincludedir)/threads

noinst_LTLIBRARIES = libfhtagn_threads.la

libfhtagn_threads_la_SOURCES = \
	tasklet.cpp

libfhtagn_threads_la_LDFLAGS = $(LIB_VERSION)

threadsinclude_HEADERS = \
	tasklet.h
