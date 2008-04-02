# Copyright (C) 2007,2008 the authors.
#
# This file is part of the Fhtagn! C++ Library, and may be distributed under
# the following license terms and conditions as set forth in the COPYING file
# included in this source distribution.

include $(top_srcdir)/Makefile.common

if HAVE_CPPUNIT

check_PROGRAMS = \
	testsuite

TEST_SRC = \
	pointers_test.cpp \
	enhanced_types_test.cpp \
	transcoding_test.cpp \
	misc_test.cpp \
	testdriver.cpp

if HAVE_BOOST
TEST_SRC += variant_test.cpp \
	threads_test.cpp
endif

testsuite_SOURCES = $(TEST_SRC)

testsuite_CXXFLAGS = \
	$(AM_CXXFLAGS) $(CPPUNIT_CFLAGS)

testsuite_LDADD = \
	$(top_builddir)/fhtagn/libfhtagn.la \
	$(top_builddir)/fhtagn/util/libfhtagn_util.la \
	$(CPPUNIT_LIBS) \
	$(BOOST_LDFLAGS) -lboost_signals -lboost_thread-mt

testsuite_LDFLAGS = $(LDFLAGS_FHTAGN)

else

check-local:
	@echo "***********************************************************" >&2
	@echo "**                                                       **" >&2
	@echo "**  You need cppunit installed to build the test suite!  **" >&2
	@echo "**                                                       **" >&2
	@echo "***********************************************************" >&2
	@exit 1

endif
