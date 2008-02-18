/**
 * $Id$
 *
 * Copyright (C) 2007 the authors.
 *
 * Author: Jens Finkhaeuser <unwesen@users.sourceforge.net>
 *
 * This file is part of the Fhtagn! C++ Library, and may be distributed under
 * the following license terms:
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 **/

#ifndef FHTAGN_UTIL_CPPUNIT_OUTPUT_H
#define FHTAGN_UTIL_CPPUNIT_OUTPUT_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <stdint.h>

#include <iostream>
#include <stack>

#include <cppunit/TestListener.h>

namespace fhtagn {
namespace util {

/**
 * The VerboseOutput class implements CppUnit's TestListener interface,
 * and produces verbose output as to which tests are currently running, and
 * whether they succeeded or not. Output is produced on a std::ostream.
 **/
class VerboseOutput
    : public CppUnit::TestListener
{
public:
    /**
     * Constructor
     *
     * @param os Output stream on which to produce verbose test output.
     * @param indent_by [default: 2] Test suites may be nested, and output for
     *    each inner test suite is indented by this amount of spaces from the
     *    output of the enclosing test suite.
     * @param max_line [default: 79] Maximum number of characters per line; this
     *    value is a hint for laying out the output, but lines may still exceed
     *    this limit in extreme cases.
     **/
    VerboseOutput(std::ostream & os, uint32_t indent_by = 2,
            uint32_t max_line = 79);

private:
    /**
     * CppUnit distinguishes between tests that succeed, that fail (i.e. where
     * an assertion is not fulfilled), and errors that were not expected by the
     * test suite.
     **/
    enum status
    {
        OK,
        FAILURE,
        ERROR
    };


    /**
     * Structure for counting successes/failures/errors of each test suite.
     **/
    struct results
    {
        uint32_t successes;
        uint32_t failures;
        uint32_t errors;
    };


    /** @see CppUnit::TestListener */
    virtual void startTest(CppUnit::Test * test);

    /** @see CppUnit::TestListener */
    virtual void addFailure(CppUnit::TestFailure const & failure);

    /** @see CppUnit::TestListener */
    virtual void endTest(CppUnit::Test * test);

    /** @see CppUnit::TestListener */
    virtual void startSuite(CppUnit::Test * suite);

    /** @see CppUnit::TestListener */
    virtual void endSuite(CppUnit::Test * suite);

    /** @see CppUnit::TestListener */
    virtual void startTestRun(CppUnit::Test * test,
            CppUnit::TestResult * eventManager);

    /** @see CppUnit::TestListener */
    virtual void endTestRun(CppUnit::Test * test,
            CppUnit::TestResult * eventManager);


    /** Used for counting results in each nested test suite. **/
    std::stack<results> m_results;

    /**
     * Result of individual test runs; must be preserved across a few function
     * calls.
     **/
    status m_status;

    /** Stream to push output to. **/
    std::ostream & m_os;

    /** Indentation depth **/
    uint32_t m_indent_by;

    /** Maximum line size */
    uint32_t m_max_line;
};

}} // namespace fhtagn::util

#endif // guard
