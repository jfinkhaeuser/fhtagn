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

#include <fhtagn/stdvariant.h>

class VariantTest
    : public CppUnit::TestFixture
{
public:
    CPPUNIT_TEST_SUITE(VariantTest);

      CPPUNIT_TEST(testSimple);
      CPPUNIT_TEST(testArray);
      CPPUNIT_TEST(testMap);
      CPPUNIT_TEST(testString);
      CPPUNIT_TEST(testCheck);

    CPPUNIT_TEST_SUITE_END();
private:
    void testSimple()
    {
        fhtagn::variant x = 42;
        CPPUNIT_ASSERT(x.is<int>());
        x.as<int>() += 3;
        CPPUNIT_ASSERT(x.as<int>() == 45);

        x = 3.141592;
        CPPUNIT_ASSERT(x.is<double>());
        CPPUNIT_ASSERT(!x.is<int>());
    }

    void testArray()
    {
        fhtagn::variant y = fhtagn::variant::array_t(1);
        CPPUNIT_ASSERT(y.is<fhtagn::variant::array_t>());
        y[0] = 42;
        y[0].as<int>() += 1234;
        CPPUNIT_ASSERT(y[0].as<int>() == 1276);

        // treating y[0] as a container must fail
        CPPUNIT_ASSERT_THROW(y[0]["foo"] = 123, std::logic_error);

        // automatically promoting an undefined variant to array must fail
        fhtagn::variant x;
        CPPUNIT_ASSERT_THROW(x[0] = 42, std::logic_error);

        // try using a vector method...
        fhtagn::variant z = fhtagn::variant::array_t();
        z.as<fhtagn::variant::array_t>().push_back(12);
    }


    void testMap()
    {
        // promoting an undefined variable to map (or array) must succeed
        fhtagn::variant x;
        CPPUNIT_ASSERT_NO_THROW(x["foo"] = 123);
        CPPUNIT_ASSERT(x.is<fhtagn::variant::map_t>());
        CPPUNIT_ASSERT(x["foo"].is<int>());

        // even to more than one level
        fhtagn::variant y;
        CPPUNIT_ASSERT_NO_THROW(y["foo"]["bar"]["baz"] = 123);
        CPPUNIT_ASSERT(y.is<fhtagn::variant::map_t>());
        CPPUNIT_ASSERT(y["foo"].is<fhtagn::variant::map_t>());
        CPPUNIT_ASSERT(y["foo"]["bar"].is<fhtagn::variant::map_t>());
        CPPUNIT_ASSERT(y["foo"]["bar"]["baz"].is<int>());

        // again, as with arrays, can't change a map to another type implicitly
        CPPUNIT_ASSERT_THROW(y["foo"][0] = 123, std::logic_error);

        // but can do so explicitly
        CPPUNIT_ASSERT_NO_THROW(y["foo"] = 123);
    }


    void testString()
    {
        fhtagn::variant x;
        CPPUNIT_ASSERT_NO_THROW(x = std::string("hello"));
        CPPUNIT_ASSERT(x.is<std::string>());

        // different holder type from value type
        char * foo = "world";
        CPPUNIT_ASSERT_NO_THROW(x = foo);
        CPPUNIT_ASSERT(x.is<std::string>());

        // TODO dosn't work yet
        // CPPUNIT_ASSERT(x = "baz");
    }

    void testCheck()
    {
        fhtagn::variant x;
        CPPUNIT_ASSERT_EQUAL(false, fhtagn::variant::check<int>(x));
        CPPUNIT_ASSERT_EQUAL(false, fhtagn::variant::check<int>(x["foo"]["bar"]));

        x["foo"]["bar"] = 3.141592;
        CPPUNIT_ASSERT_EQUAL(false, fhtagn::variant::check<int>(x["foo"]["bar"]));

        x["foo"]["bar"] = 3;
        CPPUNIT_ASSERT_EQUAL(true, fhtagn::variant::check<int>(x["foo"]["bar"]));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(VariantTest);
