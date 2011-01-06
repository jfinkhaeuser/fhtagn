/**
 * $Id$
 *
 * This file is part of the Fhtagn! C++ Library.
 * Copyright (C) 2011 Jens Finkhaeuser <unwesen@users.sourceforge.net>.
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
#ifndef FHTAGN_CLOSURE_H
#define FHTAGN_CLOSURE_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <fhtagn/fhtagn.h>

#include <fhtagn/shared_ptr.h>

#include <boost/function.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/facilities/intercept.hpp>
#include <boost/preprocessor/iteration/local.hpp>

#include <boost/any.hpp>

namespace fhtagn {

/**
 * Defines a closure type, to be constructed via a make_closure() function. As
 * opposed to e.g. boost::function, this type is intended to a) bind arguments
 * on construction, and b) erase argument types.
 *
 * Usage:
 *  closure c1 = make_closure<int, bool, long>(&funcptr, bool_arg, long_arg);
 *  closure c2 = make_closure<double, int, double>(functor(), int_arg, double_arg);
 *
 *  c1();
 *  c2();
 *
 *  int res1 = c1.result<int>();
 *  double res2 = c2.result<double>();
 *
 * The first template argument to make_closure() is the return type of the bound
 * function, and must always be provided even if the return type is void.
 * Other template arguments are the parameter types to the bound function, and
 * also must be provided unless the function accepts no parameters.
 *
 * As you can see, the make_closure() function is not as convenient as e.g.
 * boost::make_tuple or std::make_pair. Chalk this down to laziness on my
 * part.
 *
 * Arguments are bound on closure construction, and copied to an internal
 * structure along with the function pointer or functor. Regular copy semantics
 * are used when copying arguments, function pointers or functors, i.e.
 *
 *   a) POD-types, structures, etc. are copied
 *   b) pointers are copied, but not pointees
 *   c) functors are copied; use boost::ref if you don't want that.
 *
 * If you e.g. use pointers, boost::ref, etc. to avoid creating copies, note
 * that your original objects must still be in scope when the closure is
 * invoked.
 *
 * Note that if you copy closure objects, bound functors and arguments are not
 * copied, but shared between the copies.
 **/
struct closure
{
    template <
      typename closureT
    >
    closure(closureT const & closure)
      : m_closure(new closureT(closure))
    {
    }

    void operator()()
    {
      m_closure->invoke();
    }

    template <
      typename T
    >
    T
    result() const
    {
      return boost::any_cast<T>(m_closure->m_result);
    }


    struct closure_base
    {
      virtual void invoke() = 0;
      boost::any  m_result;
    };

private:
    fhtagn::shared_ptr<closure_base>  m_closure;
};


template <
  typename R,
  typename T0,
  typename T1,
  typename T2,
  typename T3,
  typename T4,
  typename T5,
  typename T6,
  typename T7,
  typename T8,
  typename T9
>
struct closure_wrapper
    : public closure::closure_base
{
  typedef boost::function10<R, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> func_type;

  closure_wrapper(func_type func, T0 const & t0, T1 const & t1, T2 const & t2,
      T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7,
      T8 const & t8, T9 const & t9)
    : m_func(func)
    , m_t0(t0)
    , m_t1(t1)
    , m_t2(t2)
    , m_t3(t3)
    , m_t4(t4)
    , m_t5(t5)
    , m_t6(t6)
    , m_t7(t7)
    , m_t8(t8)
    , m_t9(t9)
  {
  }

  virtual void invoke()
  {
    m_result = m_func(m_t0, m_t1, m_t2, m_t3, m_t4, m_t5, m_t6, m_t7, m_t8, m_t9);
  }

  func_type   m_func;
  T0          m_t0;
  T1          m_t1;
  T2          m_t2;
  T3          m_t3;
  T4          m_t4;
  T5          m_t5;
  T6          m_t6;
  T7          m_t7;
  T8          m_t8;
  T9          m_t9;
};

#define FHTAGN_CLOSURE_WRAPPER_SPECIALIZED(z, n, unused) , void
#define FHTAGN_CLOSURE_WRAPPER_MEMBER(z, n, unused) BOOST_PP_CAT(T, n) BOOST_PP_CAT(m_t, n);
#define FHTAGN_CLOSURE_WRAPPER_INITIALIZER(z, n, unused) , BOOST_PP_CAT(m_t, n)(BOOST_PP_CAT(t, n))

#define FHTAGN_CLOSURE_WRAPPER_SPECIALIZATION(z, n, unused)                         \
  template <                                                                        \
    typename R BOOST_PP_COMMA_IF(n) \
    BOOST_PP_ENUM_PARAMS(n, typename T)                                             \
  >                                                                                 \
  struct closure_wrapper<                                                           \
    R BOOST_PP_COMMA_IF(n) \
    BOOST_PP_ENUM_PARAMS(n, T)                                                      \
    BOOST_PP_REPEAT(BOOST_PP_SUB(10, n), FHTAGN_CLOSURE_WRAPPER_SPECIALIZED, -)     \
  > : public closure::closure_base                                                  \
  {                                                                                 \
    typedef boost::BOOST_PP_CAT(function, n) <                                      \
      R BOOST_PP_COMMA_IF(n) \
      BOOST_PP_ENUM_PARAMS(n, T)                                                    \
    > func_type;                                                                    \
                                                                                    \
    func_type m_func;                                                               \
    BOOST_PP_REPEAT(n, FHTAGN_CLOSURE_WRAPPER_MEMBER, -)                            \
                                                                                    \
    closure_wrapper(func_type func BOOST_PP_COMMA_IF(n) \
        BOOST_PP_ENUM_BINARY_PARAMS(n, T, const & t))   \
      : m_func(func)                                                                \
      BOOST_PP_REPEAT(n, FHTAGN_CLOSURE_WRAPPER_INITIALIZER, -)                     \
    {                                                                               \
    }                                                                               \
                                                                                    \
    virtual void invoke()                                                           \
    {                                                                               \
      m_result = m_func(BOOST_PP_ENUM_PARAMS(n, m_t));                              \
    }                                                                               \
  };

#define BOOST_PP_LOCAL_MACRO(n) FHTAGN_CLOSURE_WRAPPER_SPECIALIZATION(~, n, ~)
#define BOOST_PP_LOCAL_LIMITS   (0, 9)
#include BOOST_PP_LOCAL_ITERATE()

#undef FHTAGN_CLOSURE_WRAPPER_SPECIALIZATION
#define FHTAGN_CLOSURE_WRAPPER_SPECIALIZATION(z, n, unused)                         \
  template <                                                                        \
    BOOST_PP_ENUM_PARAMS(n, typename T)                                             \
  >                                                                                 \
  struct closure_wrapper<                                                           \
    void BOOST_PP_COMMA_IF(n) \
    BOOST_PP_ENUM_PARAMS(n, T)                                                      \
    BOOST_PP_REPEAT(BOOST_PP_SUB(10, n), FHTAGN_CLOSURE_WRAPPER_SPECIALIZED, -)     \
  > : public closure::closure_base                                                  \
  {                                                                                 \
    typedef boost::BOOST_PP_CAT(function, n) <                                      \
      void BOOST_PP_COMMA_IF(n) \
      BOOST_PP_ENUM_PARAMS(n, T)                                                    \
    > func_type;                                                                    \
                                                                                    \
    func_type m_func;                                                               \
    BOOST_PP_REPEAT(n, FHTAGN_CLOSURE_WRAPPER_MEMBER, -)                            \
                                                                                    \
    closure_wrapper(func_type func BOOST_PP_COMMA_IF(n) \
        BOOST_PP_ENUM_BINARY_PARAMS(n, T, const & t))   \
      : m_func(func)                                                                \
      BOOST_PP_REPEAT(n, FHTAGN_CLOSURE_WRAPPER_INITIALIZER, -)                     \
    {                                                                               \
    }                                                                               \
                                                                                    \
    virtual void invoke()                                                           \
    {                                                                               \
      m_func(BOOST_PP_ENUM_PARAMS(n, m_t));                                         \
    }                                                                               \
  };

#define BOOST_PP_LOCAL_MACRO(n) FHTAGN_CLOSURE_WRAPPER_SPECIALIZATION(~, n, ~)
#define BOOST_PP_LOCAL_LIMITS   (0, 10)
#include BOOST_PP_LOCAL_ITERATE()

#undef FHTAGN_CLOSURE_WRAPPER_MEMBER
#undef FHTAGN_CLOSURE_WRAPPER_INITIALIZER
#undef FHTAGN_CLOSURE_WRAPPER_SPECIALIZATION


#define FHTAGN_MAKE_CLOSURE(z, n, unused)                                           \
  template <                                                                        \
    typename R BOOST_PP_COMMA_IF(n) \
    BOOST_PP_ENUM_PARAMS(n, typename T)                                             \
  >                                                                                 \
  closure                                                                           \
  make_closure(                                                                     \
      typename closure_wrapper<                                                     \
        R BOOST_PP_COMMA_IF(n) \
        BOOST_PP_ENUM_PARAMS(n, T)                                                  \
        BOOST_PP_REPEAT(BOOST_PP_SUB(10, n), FHTAGN_CLOSURE_WRAPPER_SPECIALIZED, -) \
      >::func_type func BOOST_PP_COMMA_IF(n) \
      BOOST_PP_ENUM_BINARY_PARAMS(n, T, const & t)                                  \
  )                                                                                 \
  {                                                                                 \
    return closure_wrapper<                                                         \
      R BOOST_PP_COMMA_IF(n) \
      BOOST_PP_ENUM_PARAMS(n, T)                                                    \
      BOOST_PP_REPEAT(BOOST_PP_SUB(10, n), FHTAGN_CLOSURE_WRAPPER_SPECIALIZED, -)   \
    >(                                                                              \
        func BOOST_PP_COMMA_IF(n) \
        BOOST_PP_ENUM_PARAMS(n, t)                                                  \
    );                                                                              \
  }

#define BOOST_PP_LOCAL_MACRO(n) FHTAGN_MAKE_CLOSURE(~, n, ~)
#define BOOST_PP_LOCAL_LIMITS   (0, 10)
#include BOOST_PP_LOCAL_ITERATE()

#undef FHTAGN_CLOSURE_WRAPPER_SPECIALIZED
#undef FHTAGN_MAKE_CLOSURE
#undef BOOST_PP_LOCAL_MACRO
#undef BOOST_PP_LOCAL_LIMITS


} // namespace fhtagn

#endif // guard
