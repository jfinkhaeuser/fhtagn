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
gencov: run-tests
	@$(top_srcdir)/gencov.sh "$(srcdir)" "$(builddir)"
endif

EXTRA_DIST = \
	Makefile.common bootstrap.sh gencov.sh
