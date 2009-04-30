/**
 * $Id$
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
#ifndef FHTAGN_META_FOR_H
#define FHTAGN_META_FOR_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <fhtagn/fhtagn.h>

namespace fhtagn {
namespace meta {

/**
 * Some possible constructs for incrementing the counter in the compile-time for
 * loops below.
 **/
template <int CURRENT, int DIFF>
struct increment
{
  enum {
    value = CURRENT + DIFF,
  };
};


template <int CURRENT, int DIFF>
struct decrement
{
  enum {
    value = CURRENT - DIFF,
  };
};


template <int CURRENT>
struct inc_once : public increment<CURRENT, 1>
{
};


template <int CURRENT>
struct dec_once : public decrement<CURRENT, 1>
{
};


template <int CURRENT, int FACTOR>
struct multiply
{
  enum {
    value = CURRENT * FACTOR,
  };
};


template <int CURRENT>
struct multi_double : public multiply<CURRENT, 2>
{
};


template <int CURRENT, int DIVISOR>
struct divide
{
  enum {
    value = CURRENT / DIVISOR,
  };
};





/**
 * Of all metaprogramming techniques, a compile-time for loop is probably the
 * best explored. This file offers two implementations with slightly different
 * use-cases. They're probably not much better or worse than other
 * implementations out there, but at least they don't pull in additional
 * dependencies.
 *
 * The first implementation, dynamic_for, iterates at compile-time over a range
 * of integer values defined by [START, END). For each of the values, a functor
 * is called, to which the current iteration value is passed as a run-time
 * parameter:
 *
 *    void functor(int i);
 *    dynamic_for<0, 10>(&functor);
 *
 *    struct functor2
 *    {
 *      void operator()(int i);
 *    };
 *    functor2 f;
 *    dynamic_for<0, 10>(f);
 *
 * In the above examples, the functor is called 10 times, with values ranging
 * from 0 to 9. If you wish to pass more values than the current iteration
 * value to the functor, you should probably take a look at boost::bind.
 *
 * The dynamic_for construct allows for an optional template parameter to change
 * how the loop increments the current iteration value. By default, the inc_once
 * structure above is used.
 *
 * Finally, dynamic_for allows for an optional comparator template parameter, to
 * change how the current loop value is compared to the end loop value. By
 * default, a less operation is performed, to follow the runtime for loop
 * semantics. Alternatively, you can also choose to supply any of the
 * comparators from fhtagn/meta/comparison.h, or your custom constructs.
 **/
template <
  int START,
  int END,
  typename functorT
>
inline void
dynamic_for(functorT const & func);


template <
  int START,
  int END,
  template <int> class incrementorT,
  typename functorT
>
inline void
dynamic_for(functorT const & func);


template <
  int START,
  int END,
  template <int, int> class comparatorT,
  typename functorT
>
inline void
dynamic_for(functorT const & func);


template <
  int START,
  int END,
  template <int> class incrementorT,
  template <int, int> class comparatorT,
  typename functorT
>
inline void
dynamic_for(functorT const & func);


template <
  int START,
  int END,
  typename functorT
>
inline void
dynamic_for(functorT & func);


template <
  int START,
  int END,
  template <int> class incrementorT,
  typename functorT
>
inline void
dynamic_for(functorT & func);


template <
  int START,
  int END,
  template <int, int> class comparatorT,
  typename functorT
>
inline void
dynamic_for(functorT & func);


template <
  int START,
  int END,
  template <int> class incrementorT,
  template <int, int> class comparatorT,
  typename functorT
>
inline void
dynamic_for(functorT & func);




/**
 * The second implementation, static_for, performs the same iteration. Instead
 * of calling the same functor over and over, though, it instanciates a class
 * template you specify with the current iteration value as it's template
 * parameter:
 *
 *    template <int I>
 *    struct functor
 *    {
 *      void operator()();
 *    };
 *
 *    static_for<0, 10, functor>();
 *
 * In the above example, a functor<0>, functor<1>, ..., functor<9> are
 * instanciated and their respective operator()s called. Note that static_for
 * only works with struct-based functors, not first-class functions.
 *
 * If you wish to pass additional parameters to the functor's operator()s, there
 * is support for an additional parameter. If you wish to pass more than one
 * additional parameters, consider passing a boost::tuple instead:
 *
 *    template <int I>
 *    struct functor
 *    {
 *      void operator()(int a);
 *    };
 *
 *    static_for<0, 10, functor>(1);
 *
 * The static_for construct allows for an optional template parameter to change
 * how the loop increments the current iteration value. By default, the inc_once
 * structure above is used.
 *
 * Finally, dynamic_for allows for an optional comparator template parameter, to
 * change how the current loop value is compared to the end loop value. By
 * default, a less operation is performed, to follow the runtime for loop
 * semantics. Alternatively, you can also choose to supply any of the
 * comparators from fhtagn/meta/comparison.h, or your custom constructs.
 **/

/**
 * No parameter
 **/
template <
  int START,
  int END,
  template <int> class functorT
>
inline void
static_for();


template <
  int START,
  int END,
  template <int> class incrementorT,
  template <int> class functorT
>
inline void
static_for();


template <
  int START,
  int END,
  template <int, int> class comparatorT,
  template <int> class functorT
>
inline void
static_for();


template <
  int START,
  int END,
  template <int> class incrementorT,
  template <int, int> class comparatorT,
  template <int> class functorT
>
inline void
static_for();


/**
 * Parameter by reference
 **/
template <
  int START,
  int END,
  template <int> class functorT,
  typename functor_paramT
>
inline void
static_for(functor_paramT & param);


template <
  int START,
  int END,
  template <int> class incrementorT,
  template <int> class functorT,
  typename functor_paramT
>
inline void
static_for(functor_paramT & param);


template <
  int START,
  int END,
  template <int, int> class comparatorT,
  template <int> class functorT,
  typename functor_paramT
>
inline void
static_for(functor_paramT & param);


template <
  int START,
  int END,
  template <int> class incrementorT,
  template <int, int> class comparatorT,
  template <int> class functorT,
  typename functor_paramT
>
inline void
static_for(functor_paramT & param);


/**
 * Parameter by const reference
 **/
template <
  int START,
  int END,
  template <int> class functorT,
  typename functor_paramT
>
inline void
static_for(functor_paramT const & param);


template <
  int START,
  int END,
  template <int> class incrementorT,
  template <int> class functorT,
  typename functor_paramT
>
inline void
static_for(functor_paramT const & param);


template <
  int START,
  int END,
  template <int, int> class comparatorT,
  template <int> class functorT,
  typename functor_paramT
>
inline void
static_for(functor_paramT const & param);


template <
  int START,
  int END,
  template <int> class incrementorT,
  template <int, int> class comparatorT,
  template <int> class functorT,
  typename functor_paramT
>
inline void
static_for(functor_paramT const & param);



}} // namespace fhtagn::meta

#include <fhtagn/meta/detail/dynamic_for.tcc>
#include <fhtagn/meta/detail/static_for.tcc>

#endif // guard
