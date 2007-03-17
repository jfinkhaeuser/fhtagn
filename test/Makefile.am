# Copyright (C) 2007 the authors.
#
# This file is part of the Fhtagn! C++ Library, and may be distributed under
# the following license terms and conditions as set forth in the COPYING file
# included in this source distribution.

include $(top_srcdir)/Makefile.common

if HAVE_CPPUNIT

check_PROGRAMS = \
	testsuite

testsuite_SOURCES = \
	variant_test.cpp \
	pointers_test.cpp \
	testdriver.cpp

testsuite_CXXFLAGS = \
	$(AM_CXXFLAGS) $(CPPUNIT_CFLAGS)

testsuite_LDADD = \
	$(top_builddir)/fhtagn/libfhtagn.la \
	$(top_builddir)/fhtagn/util/libfhtagn_util.la \
	$(CPPUNIT_LIBS)

else

check-local:
	@echo "***********************************************************" >&2
	@echo "**                                                       **" >&2
	@echo "**  You need cppunit installed to build the test suite!  **" >&2
	@echo "**                                                       **" >&2
	@echo "***********************************************************" >&2
	@exit 1

endif
