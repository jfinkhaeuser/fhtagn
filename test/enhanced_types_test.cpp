/**
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

#include <cppunit/extensions/HelperMacros.h>

#include <fhtagn/property.h>
#include <fhtagn/mandatory.h>

namespace {

struct rw_property_test
{
    rw_property_test()
        : the_property(this, &rw_property_test::get, &rw_property_test::set)
        , m_value(0)
    {
    }

    int get() const
    {
        return m_value;
    }

    void set(int const & new_value)
    {
        m_value = new_value;
    }

    fhtagn::property<int, rw_property_test> the_property;

    int m_value;
};


fhtagn::throw_if_unchecked<int> foo()
{
    return 42;
}

} // anonymous namespace

class EnhancedTypesTest
    : public CppUnit::TestFixture
{
public:
    CPPUNIT_TEST_SUITE(EnhancedTypesTest);

        CPPUNIT_TEST(testProperty);
        CPPUNIT_TEST(testMandatory);

    CPPUNIT_TEST_SUITE_END();
private:

    void testProperty()
    {
        rw_property_test tester;
        tester.the_property = 123; // this line fails to compile for a read_only_property
        CPPUNIT_ASSERT_EQUAL(tester.m_value, 123);

        // here we're testing whether the overloaded assignment operators do
        // their job, by example of the /= operator.
        tester.the_property /= 10; // this line fails to compile for a write_only_property
        CPPUNIT_ASSERT_EQUAL(tester.m_value, 12);

        // whereas this test shows that casting a property to it's reprT, using
        // the result in a calculation, and assigning back to a property works
        // just as well.
        tester.the_property = tester.the_property * 10;
        CPPUNIT_ASSERT_EQUAL(tester.m_value, 120);
    }


    void testMandatory()
    {
        CPPUNIT_ASSERT_THROW(foo(), std::logic_error);
        CPPUNIT_ASSERT_NO_THROW(fhtagn::ignore_return_value(foo()));
        CPPUNIT_ASSERT_NO_THROW(int ret = foo());

        int ret = foo();
        CPPUNIT_ASSERT_EQUAL(ret, 42);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(EnhancedTypesTest);
