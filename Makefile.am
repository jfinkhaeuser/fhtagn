# Copyright (C) 2007 the authors.
#
# This file is part of the Fhtagn! C++ Library, and may be distributed under
# the following license terms and conditions as set forth in the COPYING file
# included in this source distribution.

SUBDIRS = fhtagn test

include $(top_srcdir)/Makefile.common

run-tests: check
	@test/testsuite

if ENABLE_COVERAGE
cleancov:
	@find "$(top_builddir)" -type f -name \*.gcda -exec rm \{\} \;

gencov: cleancov run-tests
	@"$(top_srcdir)/gencov.sh" "$(top_srcdir)" "$(top_builddir)"
endif

buildbot-install:
	DESTDIR=$(shell pwd)/buildbot-install $(MAKE) install

EXTRA_DIST = \
	Makefile.common bootstrap.sh gencov.sh libfhtagn.pc.in libfhtagn_util.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libfhtagn.pc
