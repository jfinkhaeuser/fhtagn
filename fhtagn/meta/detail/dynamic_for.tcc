/**
 * $Id: template.h 197 2008-11-02 12:02:37Z unwesen $
 *
 * Copyright (C) 2009 the authors.
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
#ifndef FHTAGN_META_DETAIL_DYNAMIC_FOR_TCC
#define FHTAGN_META_DETAIL_DYNAMIC_FOR_TCC

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <fhtagn/meta/comparison.h>

namespace fhtagn {
namespace meta {


/*****************************************************************************
 * Helper structures
 **/

namespace detail {
namespace dynamic_for_helpers {

/**
 * Generic implementation of next is for cases where the TRUE_FALSE condition is
 * non-zero, i.e. true.
 *
 * In those cases, we first call the functor and pass the CURRENT value. Then we
 * instanciate a new next with CURRENT incremented, and pass control to that
 * instance's run function.
 **/
template <
  int TRUE_FALSE, // result of exit condition; next is specialized on this below
  int CURRENT,    // current value of the for loop counter
  int END,        // end of the for loop
  template <int> class incrementorT,
  template <int, int> class comparatorT
>
struct next
{
  template <typename functorT>
  inline static void run(functorT & func)
  {
    func(CURRENT);
    next<
      comparatorT<incrementorT<CURRENT>::value, END>::value,
      incrementorT<CURRENT>::value,
      END,
      incrementorT,
      comparatorT
    >::run(func);
  }

  template <typename functorT>
  inline static void run(functorT const & func)
  {
    func(CURRENT);
    next<
      comparatorT<incrementorT<CURRENT>::value, END>::value,
      incrementorT<CURRENT>::value,
      END,
      incrementorT,
      comparatorT
    >::run(func);
  }
};


/**
 * Specialization for TRUE_FALSE == 0 - neither run function does anything, as
 * END is not included in the iterated range.
 **/
template <
  int CURRENT,
  int END,
  template <int> class incrementorT,
  template <int, int> class comparatorT
>
struct next<0, CURRENT, END, incrementorT, comparatorT>
{
  template <typename functorT>
  inline static void run(functorT & f)
  {
  }

  template <typename functorT>
  inline static void run(functorT const & f)
  {
  }
};


}} // namespace detail::dynamic_for_helpers

/*****************************************************************************
 * Wrapper function implementations
 **/
template <
  int START,
  int END,
  typename functorT
>
inline void
dynamic_for(functorT const & func)
{
  detail::dynamic_for_helpers::next<
    ::fhtagn::meta::less<START, END>::value,
    START,
    END,
    ::fhtagn::meta::inc_once,
    ::fhtagn::meta::less
  >::template run<functorT>(func);
}


template <
  int START,
  int END,
  template <int> class incrementorT,
  typename functorT
>
inline void
dynamic_for(functorT const & func)
{
  detail::dynamic_for_helpers::next<
    ::fhtagn::meta::less<START, END>::value,
    START,
    END,
    incrementorT,
    ::fhtagn::meta::less
  >::template run<functorT>(func);
}


template <
  int START,
  int END,
  template <int, int> class comparatorT,
  typename functorT
>
inline void
dynamic_for(functorT const & func)
{
  detail::dynamic_for_helpers::next<
    comparatorT<START, END>::value,
    START,
    END,
    ::fhtagn::meta::inc_once,
    comparatorT
  >::template run<functorT>(func);
}


template <
  int START,
  int END,
  template <int> class incrementorT,
  template <int, int> class comparatorT,
  typename functorT
>
inline void
dynamic_for(functorT const & func)
{
  detail::dynamic_for_helpers::next<
    comparatorT<START, END>::value,
    START,
    END,
    incrementorT,
    comparatorT
  >::template run<functorT>(func);
}


template <
  int START,
  int END,
  typename functorT
>
inline void
dynamic_for(functorT & func)
{
  detail::dynamic_for_helpers::next<
    ::fhtagn::meta::less<START, END>::value,
    START,
    END,
    ::fhtagn::meta::inc_once,
    ::fhtagn::meta::less
  >::template run<functorT>(func);
}


template <
  int START,
  int END,
  template <int> class incrementorT,
  typename functorT
>
inline void
dynamic_for(functorT & func)
{
  detail::dynamic_for_helpers::next<
    ::fhtagn::meta::less<START, END>::value,
    START,
    END,
    incrementorT,
    ::fhtagn::meta::less
  >::template run<functorT>(func);
}


template <
  int START,
  int END,
  template <int, int> class comparatorT,
  typename functorT
>
inline void
dynamic_for(functorT & func)
{
  detail::dynamic_for_helpers::next<
    comparatorT<START, END>::value,
    START,
    END,
    ::fhtagn::meta::inc_once,
    comparatorT
  >::template run<functorT>(func);
}


template <
  int START,
  int END,
  template <int> class incrementorT,
  template <int, int> class comparatorT,
  typename functorT
>
inline void
dynamic_for(functorT & func)
{
  detail::dynamic_for_helpers::next<
    comparatorT<START, END>::value,
    START,
    END,
    incrementorT,
    comparatorT
  >::template run<functorT>(func);
}

}} // namespace fhtagn::meta

#endif // guard
