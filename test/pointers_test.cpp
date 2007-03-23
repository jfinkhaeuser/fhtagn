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

#include <cppunit/extensions/HelperMacros.h>

#include <fhtagn/pointers.h>

namespace {

struct pointer_test_struct
{
    pointer_test_struct()
        : m_counter(0)
    {
    }

    void inc() const
    {
        ++m_counter;
    }

    mutable int m_counter;
};

template <template <typename> class pointerT>
void pointer_tester(typename pointerT<pointer_test_struct>::type param)
{
    param->inc();
}


struct copy_policy_test_struct
{
    copy_policy_test_struct()
        : m_copied(false)
    {
    }

    copy_policy_test_struct(copy_policy_test_struct const & other)
        : m_copied(true)
    {
    }

    bool m_copied;
};

} // anonymous namespace

class PointerPolicyTest
    : public CppUnit::TestFixture
{
public:
    CPPUNIT_TEST_SUITE(PointerPolicyTest);

        CPPUNIT_TEST(testPointerTypes);
        CPPUNIT_TEST(testShallowCopyPolicy);
        CPPUNIT_TEST(testDeepCopyPolicy);

    CPPUNIT_TEST_SUITE_END();
private:

    void testPointerTypes()
    {
        // test that pointer_tester compiles and works on all types of pointers...
        pointer_test_struct x;

        pointer_tester<fhtagn::pointers::pointer>(&x);
        CPPUNIT_ASSERT_EQUAL(x.m_counter, 1);

        pointer_tester<fhtagn::pointers::const_pointer>(&x);
        CPPUNIT_ASSERT_EQUAL(x.m_counter, 2);

        pointer_tester<fhtagn::pointers::const_pointer_const>(&x);
        CPPUNIT_ASSERT_EQUAL(x.m_counter, 3);
    }


    void testShallowCopyPolicy()
    {
        using namespace fhtagn::pointers;

        copy_policy_test_struct * x = new copy_policy_test_struct();
        CPPUNIT_ASSERT_EQUAL(x->m_copied, false);

        shallow_copy<copy_policy_test_struct, pointer> first = x;
        CPPUNIT_ASSERT_EQUAL(first.m_pointer->m_copied, false);

        // after copying shallow_copy, it's embedded m_pointer must not have been
        // copy constructed.
        shallow_copy<copy_policy_test_struct, pointer> second = first;
        CPPUNIT_ASSERT_EQUAL(second.m_pointer->m_copied, false);

        delete x;
    }


    void testDeepCopyPolicy()
    {
        using namespace fhtagn::pointers;

        copy_policy_test_struct * x = new copy_policy_test_struct();
        CPPUNIT_ASSERT_EQUAL(x->m_copied, false);

        deep_copy<copy_policy_test_struct, pointer> first = x;
        CPPUNIT_ASSERT_EQUAL(first.m_pointer->m_copied, false);

        // after copying deep_copy, it's embedded m_pointer must have been
        // copy constructed.
        deep_copy<copy_policy_test_struct, pointer> second = first;
        CPPUNIT_ASSERT_EQUAL(second.m_pointer->m_copied, true);

        delete x;
    }
};


CPPUNIT_TEST_SUITE_REGISTRATION(PointerPolicyTest);
