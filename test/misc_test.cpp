/**
 * $Id$
 *
 * This file is part of the Fhtagn! C++ Library.
 * Copyright (C) 2009 Jens Finkhaeuser <unwesen@users.sourceforge.net>.
 *
 * Author: Jens Finkhaeuser <unwesen@users.sourceforge.net>
 *
 * This program is licensed as free software for personal, educational or
 * other non-commerical uses: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Alternatively, licenses for commercial purposes are available as well.
 * Please send your enquiries to the copyright holder's address above.
 **/

#include <cppunit/extensions/HelperMacros.h>

#include <fhtagn/byteorder.h>
#include <fhtagn/patterns/singleton.h>
#include <fhtagn/compressed_value.h>
#include <fhtagn/closure.h>

#include <boost/bind.hpp>

namespace {

struct singleton_default_ctor
{
  singleton_default_ctor()
  {
    instanciated = true;
  }

  static bool instanciated;
};

bool singleton_default_ctor::instanciated = false;


struct singleton_non_default_ctor
{
  singleton_non_default_ctor(int a)
  {
    instanciated = true;
  }

  static bool instanciated;
};

bool singleton_non_default_ctor::instanciated = false;


struct auto_creator
{
  auto_creator()
  {
    instanciated = true;
  }


  singleton_non_default_ctor * create()
  {
    created = true;
    return new singleton_non_default_ctor(42);
  }

  static bool instanciated;
  static bool created;
};

bool auto_creator::instanciated = false;
bool auto_creator::created = false;


struct manual_creator
{
  manual_creator()
    : created(false)
  {
  }

  singleton_non_default_ctor * create()
  {
    created = true;
    return new singleton_non_default_ctor(42);
  }

  bool created;
};



struct large_value_t
{
  large_value_t()
    : p1(NULL)
    , v()
  {
  }

  void * p1;
  boost::uint8_t v;
};




static int void_result = 0;
void void_add(int a, int b)
{
  void_result = a + b;
}


int add(int a, int b)
{
  return a + b;
}


double subtract(double a, double b)
{
  return a - b;
}


double no_args()
{
  return 3.14;
}

static int void_no_args_result = 0;
void void_no_args()
{
  void_no_args_result = 42;
}

struct divide
{
  divide()
    : m_x(0)
  {
  }

  divide(divide const &)
    : m_x(-1)
  {
  }

  double operator()(double a, int b)
  {
    return a / b;
  }

  int asdf(double a, double b)
  {
    return m_x;
  }

  int m_x;
};


} // anonymous namespace

FHTAGN_SINGLETON_INITIALIZE

class MiscTest
    : public CppUnit::TestFixture
{
public:
    CPPUNIT_TEST_SUITE(MiscTest);

        CPPUNIT_TEST(testByteOrder);
        CPPUNIT_TEST(testSingleton);
        CPPUNIT_TEST(testCompressedValue);
        CPPUNIT_TEST(testClosures);

    CPPUNIT_TEST_SUITE_END();
private:

    void testByteOrder()
    {
        namespace b = fhtagn::byte_order;

        boost::uint16_t x = 1234;
        CPPUNIT_ASSERT_EQUAL(x, b::convert<>::ntoh(b::convert<>::hton(x)));
        boost::uint32_t y = 1234;
        CPPUNIT_ASSERT_EQUAL(y, b::convert<>::ntoh(b::convert<>::hton(y)));
        boost::uint64_t z = 1234;
        CPPUNIT_ASSERT_EQUAL(z, b::convert<>::ntoh(b::convert<>::hton(z)));

        if (b::host_byte_order() == b::FHTAGN_BIG_ENDIAN) {
            CPPUNIT_ASSERT_EQUAL(static_cast<boost::uint16_t>(1234), b::to_host(x, b::FHTAGN_BIG_ENDIAN));
            CPPUNIT_ASSERT_EQUAL(static_cast<boost::uint16_t>(53764), b::to_host(x, b::FHTAGN_LITTLE_ENDIAN));
        } else {
            CPPUNIT_ASSERT_EQUAL(static_cast<boost::uint16_t>(53764), b::to_host(x, b::FHTAGN_BIG_ENDIAN));
            CPPUNIT_ASSERT_EQUAL(static_cast<boost::uint16_t>(1234), b::to_host(x, b::FHTAGN_LITTLE_ENDIAN));
        }
    }

    void testSingleton()
    {
      // Not much of a test, really. We can't really test tearing down of a
      // singleton, so this code only tests for creation and the existence
      // of functions.
      {
        typedef fhtagn::patterns::singleton<singleton_default_ctor> singleton;

        CPPUNIT_ASSERT_EQUAL(false, singleton_default_ctor::instanciated);
        singleton::shared_ptr s = singleton::instance();
        CPPUNIT_ASSERT_EQUAL(true, singleton_default_ctor::instanciated);
      }

      // This additional code tests for the use of a automatically created creator.
      {
        typedef fhtagn::patterns::singleton<singleton_non_default_ctor, auto_creator> singleton;

        CPPUNIT_ASSERT_EQUAL(false, auto_creator::instanciated);
        CPPUNIT_ASSERT_EQUAL(false, auto_creator::created);
        CPPUNIT_ASSERT_EQUAL(false, singleton_non_default_ctor::instanciated);
        singleton::shared_ptr s = singleton::instance();
        CPPUNIT_ASSERT_EQUAL(true, auto_creator::instanciated);
        CPPUNIT_ASSERT_EQUAL(true, auto_creator::created);
        CPPUNIT_ASSERT_EQUAL(true, singleton_non_default_ctor::instanciated);

        // Fixup for next test.
        singleton_non_default_ctor::instanciated = false;
      }

      // This code tests for the use of a manually created creator.
      {
        typedef fhtagn::patterns::singleton<singleton_non_default_ctor, manual_creator> singleton;

        singleton::creator_shared_ptr creator(new manual_creator());
        singleton::set_creator(creator);

        CPPUNIT_ASSERT_EQUAL(false, creator->created);
        CPPUNIT_ASSERT_EQUAL(false, singleton_non_default_ctor::instanciated);
        singleton::shared_ptr s = singleton::instance();
        CPPUNIT_ASSERT_EQUAL(true, creator->created);
        CPPUNIT_ASSERT_EQUAL(true, singleton_non_default_ctor::instanciated);

        // Reset the creator's flag. A subsequent call to instance() will then
        // not invoke the creator, as a singleton instance already exists.
        creator->created = false;
        singleton::shared_ptr s2 = singleton::instance();
        CPPUNIT_ASSERT_EQUAL(false, creator->created);
      }

    }



    void testCompressedValue()
    {
      // It's a very simple test, really. We just assert that regardless of the
      // size of the value type, the size of the compressed_value is
      // smaller or equal to a pointer size.
      typedef boost::uint8_t small_value_t;

      fhtagn::compressed_value<small_value_t> cv1;
      CPPUNIT_ASSERT(sizeof(cv1) <= sizeof(void *));
      CPPUNIT_ASSERT_EQUAL(boost::uint8_t(0), *cv1);
      *cv1 = 123;
      CPPUNIT_ASSERT_EQUAL(boost::uint8_t(123), *cv1);

      fhtagn::compressed_value<large_value_t> cv2;
      CPPUNIT_ASSERT(sizeof(cv2) <= sizeof(void *));
      CPPUNIT_ASSERT_EQUAL(boost::uint8_t(0), cv2->v);
      cv2->v = 123;
      CPPUNIT_ASSERT_EQUAL(boost::uint8_t(123), cv2->v);

      // And now we'll test copy construction and assignment, and we're done.
      fhtagn::compressed_value<small_value_t> cv3(cv1);
      CPPUNIT_ASSERT_EQUAL(boost::uint8_t(123), *cv3);
      *cv1 = 42;
      cv3 = cv1;
      CPPUNIT_ASSERT_EQUAL(boost::uint8_t(42), *cv3);

    }



    void testClosures()
    {
      // Test that a function returning ints succeeds, and the result can be
      // obtained.
      fhtagn::closure c1 = fhtagn::make_closure<int, int, int>(&add, 1, 2);
      int result = 0;
      CPPUNIT_ASSERT_THROW(result = c1.result<int>(), boost::bad_any_cast);
      c1();
      CPPUNIT_ASSERT_NO_THROW(result = c1.result<int>());
      CPPUNIT_ASSERT_EQUAL(3, result);

      // Test that a function returning void can be used.
      c1 = fhtagn::make_closure<void, int, int>(&void_add, 1, 2);
      CPPUNIT_ASSERT(3 != void_result);
      c1();
      CPPUNIT_ASSERT_THROW(result = c1.result<int>(), boost::bad_any_cast);
      CPPUNIT_ASSERT_EQUAL(3, void_result);

      // Test that functions without arguments work.
      c1 = fhtagn::make_closure<double>(&no_args);
      double result2 = 0;
      c1();
      CPPUNIT_ASSERT_NO_THROW(result2 = c1.result<double>());
      CPPUNIT_ASSERT_EQUAL(3.14, result2);

      c1 = fhtagn::make_closure<void>(&void_no_args);
      CPPUNIT_ASSERT_EQUAL(0, void_no_args_result);
      c1();
      CPPUNIT_ASSERT_EQUAL(42, void_no_args_result);
      c1();

      // Test functors.
      c1 = fhtagn::make_closure<double, double, int>(divide(), 13, 2);
      c1();
      CPPUNIT_ASSERT_NO_THROW(result2 = c1.result<double>());
      CPPUNIT_ASSERT_EQUAL(6.5, result2);

      // Test bound member functions
      divide d;
      c1 = fhtagn::make_closure<int, double, double>(boost::bind(&divide::asdf, d, _1, _2), 3.14, 123);
      c1();
      CPPUNIT_ASSERT_NO_THROW(result = c1.result<int>());
      CPPUNIT_ASSERT_EQUAL(-1, result); // Because the functor was copied

      c1 = fhtagn::make_closure<int, double, double>(boost::bind(&divide::asdf, boost::ref(d), _1, _2), 3.14, 123);
      c1();
      CPPUNIT_ASSERT_NO_THROW(result = c1.result<int>());
      CPPUNIT_ASSERT_EQUAL(0, result); // Because we used boost::ref
    }
};


CPPUNIT_TEST_SUITE_REGISTRATION(MiscTest);
