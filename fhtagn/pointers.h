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
#ifndef FHTAGN_POINTERS_H
#define FHTAGN_POINTERS_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

namespace fhtagn {
namespace pointers {

/**
 * The pointer and const_pointer structs simplify writing template code that
 * should work both with pointers and const pointers. It avoids hardcoding the
 * type of pointer to use:
 *
 * template <typename T, template <typename> class pointer_type>
 * void foo(pointer_type<T>::type bar)
 * {
 *    bar->doSomething();
 * }
 *
 * int const * blargh = ...
 * foo<int, fhtagn::pointers::const_pointer>(blargh);
 *
 * You probably don't need this sort of thing unless you're writing nifty
 * template code like the pointer policies below...
 **/
template <typename T>
struct pointer
{
    typedef T * type;
};

template <typename T>
struct const_pointer
{
    typedef T const * type;
};

template <typename T>
struct const_pointer_const
{
    typedef T const * const type;
};

/**
 * Pointer policies solve a common problem, not by providing particularily
 * clever or complex code, but by giving it a name. There's very little magic
 * here...
 *
 * When handing any sort of object to a class's constructor, and that class
 * keeps a reference or pointer to that object, you always face the same
 * problems:
 *
 *  1) Who owns the object? (that's the obvious problem)
 *  2) What are the semnatics for this object, if the enclosing class gets
 *     copy constructed or assigned?
 *
 * Pointer policies answer the second question, by giving different approaches
 * a different name. Some answers to the second question imply how the first
 * question is answered.
 *
 * It's still up to you to decide which approach to use.
 **/
template <
    typename dataT,
    template <typename> class pointer_type = const_pointer
>
struct shallow_copy
{
    typename pointer_type<dataT>::type m_pointer;

    shallow_copy(typename pointer_type<dataT>::type pointer)
        : m_pointer(pointer)
    {
    }


    shallow_copy(shallow_copy const & other)
        : m_pointer(other.m_pointer)
    {
    }


    shallow_copy & operator=(shallow_copy const & other)
    {
        m_pointer = other.m_pointer;
        return *this;
    }
};


template <
    typename dataT,
    template <typename> class pointer_type = const_pointer
>
struct deep_copy
{
    typename pointer_type<dataT>::type m_pointer;

    /**
     * Must distinguish between copy-constructed and non-copy-constructed
     * instances of deep_copy - copy-constructed instances must take ownership
     * of m_pointer, because they are allocated within one of the deep_copy
     * functions.
     **/
    bool m_owner;

    deep_copy(typename pointer_type<dataT>::type pointer)
        : m_pointer(pointer)
        , m_owner(false)
    {
    }


    deep_copy(deep_copy const & other)
        : m_pointer(new dataT(*other.m_pointer))
        , m_owner(true)
    {
    }


    ~deep_copy()
    {
        if (m_owner) {
            delete m_pointer;
        }
    }


    deep_copy & operator=(deep_copy const & other)
    {
        if (m_owner) {
            delete m_pointer;
        }
        m_pointer = new dataT(*other.m_pointer);
        m_owner = true;
        return *this;
    }
};



}} // namespace fhtagn::pointers

#endif // guard
