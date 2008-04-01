/**
 * $Id$
 *
 * Copyright (C) 2007,2008 the authors.
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
#ifndef FHTAGN_BYTEORDER_H
#define FHTAGN_BYTEORDER_H

#include <stdint.h>

#include <fhtagn/config.h>

/**
 * If we can grab the byte order from the compiler, do that - if not, try to
 * use boost's facilities.
 **/
#if defined(__GNUC__)
   // We have GCC, which means we get our endianness via autoconf's
   // WORDS_BIGENDIAN define.
#  if defined(WORDS_BIGENDIAN)
#    define FHTAGN_BYTE_ORDER_TMP 4321
#  else // WORDS_BIGENDIAN
#    define FHTAGN_BYTE_ORDER_TMP 1234
#  endif // WORDS_BIGENDIAN
#else // __GNUC__
   // We don't have GCC... so far we're not explicitly supporting other compilers,
   // so we try to fall back on what boost provides. However, we need boost for
   // that purpose.
#  if defined(HAVE_BOOST)
#    include <boost/detail/endian.hpp>
#    define FHTAGN_BYTE_ORDER_TMP BOOST_BYTE_ORDER
#  else // HAVE_BOOST
#    error "Could not determine byte order either from the compiler or from boost."
#  endif // HAVE_BOOST
#endif // __GNUC__


namespace fhtagn {
namespace byte_order {

/**
 * Definition of FHTAGN_BIG_ENDIAN and FHTAGN_LITTLE_ENDIAN enum values.
 **/

enum endian
{
    FHTAGN_UNKNOWN_ENDIAN = -1,
    FHTAGN_BIG_ENDIAN     = 0,
    FHTAGN_LITTLE_ENDIAN  = 1
};


/**
 * Used to define FHTAGN_BYTE_ORDER, see below.
 **/
namespace detail {

template <int ENDIAN>
struct host_helper {
    enum { value = FHTAGN_UNKNOWN_ENDIAN };
};

template <>
struct host_helper<4321>
{
    enum { value = FHTAGN_BIG_ENDIAN };
};

template <>
struct host_helper<1234>
{
    enum { value = FHTAGN_LITTLE_ENDIAN };
};

} // namespace detail


/**
 * In the end, host_byte_order does not add anything to FHTAGN_BYTE_ORDER_TMP -
 * except that it's either -1, 0 or 1, i.e. values that are more easily used
 * as array indices (see decision matrix in to_host() below).
 *
 * If you want to avoid warnings and manual casts, compare against the
 * return value of host_byte_order() below instead of FHTAGN_BYTE_ORDER
 * directly.
 **/
enum { FHTAGN_BYTE_ORDER = detail::host_helper<FHTAGN_BYTE_ORDER_TMP>::value };

#if defined(FHTAGN_BYTE_ORDER_TMP)
#  undef FHTAGN_BYTE_ORDER_TMP
#endif


/**
 * Simple function returning the value of FHTAGN_BYTE_ORDER cast to endian,
 * to be able to compare it against endian values without warnings (or
 * manually casting yourself).
 **/
inline endian host_byte_order()
{
  return static_cast<endian>(FHTAGN_BYTE_ORDER);
}


/**
 * Simple function to return a string representation of the passed endian value,
 * that is "LE", "BE" or "--" for unknown endianness.
 **/
inline char const * const to_string(int source_endian)
{
    return (source_endian == FHTAGN_UNKNOWN_ENDIAN
            ? "--"
            : (source_endian == FHTAGN_BIG_ENDIAN
                ? "BE"
                : "LE"
              )
            );
}


/**
 * Simple function to return the opposite of the byte order passed to it,
 * i.e. returns FHTAGN_LITTLE_ENDIAN if FHTAGN_BIG_ENDIAN is passed and
 * vice versa.
 *
 * If FHTAGN_UNKNOWN_ENDIAN is passed, the same is returned.
 **/
inline endian opposite(int arg)
{
    return (arg == FHTAGN_UNKNOWN_ENDIAN
            ? FHTAGN_UNKNOWN_ENDIAN
            : (arg == FHTAGN_BIG_ENDIAN
                ? FHTAGN_LITTLE_ENDIAN
                : FHTAGN_BIG_ENDIAN
              )
            );
}


/**
 * Swap byte order of various integer sizes.
 **/
inline uint16_t swap(uint16_t const & orig)
{
    return ((orig & 0xff00) >> 8) |
           ((orig & 0x00ff) << 8);
}

inline int16_t swap(int16_t const & orig)
{
    return swap(static_cast<uint16_t>(orig));
}

inline uint32_t swap(uint32_t const & orig)
{
    return ((orig & 0xff000000UL) >> 24) |
           ((orig & 0x00ff0000UL) >> 8)  |
           ((orig & 0x0000ff00UL) << 8)  |
           ((orig & 0x000000ffUL) << 24);
}

inline int32_t swap(int32_t const & orig)
{
    return swap(static_cast<uint32_t>(orig));
}

inline uint64_t swap(uint64_t const & orig)
{
    return ((orig & ((uint64_t) 0xff << 56)) >> 56) |
           ((orig & ((uint64_t) 0xff << 48)) >> 40) |
           ((orig & ((uint64_t) 0xff << 40)) >> 24) |
           ((orig & ((uint64_t) 0xff << 32)) >> 8)  |
           ((orig & ((uint64_t) 0xff << 24)) >> 8)  |
           ((orig & ((uint64_t) 0xff << 16)) >> 24) |
           ((orig & ((uint64_t) 0xff <<  8)) << 40) |
           ((orig &  (uint64_t) 0xff       ) << 56);

}

inline int64_t swap(int64_t const & orig)
{
    return swap(static_cast<uint64_t>(orig));
}



/**
 * Unspecialized declaration of convert. Specialized versions of convert contain
 * hton() and ntoh() functions, which accept any integer type whatsoever. That
 * makes them easier to use in heavily templated code, where the type of integer
 * is not necessarily known. The definitions above are also platform independent
 * (with the drawback that platform dependent specializations /may/ exist and be
 * faster - though that's fairly unlikely).
 **/
template <int ENDIAN = FHTAGN_BYTE_ORDER>
struct convert
{
};



/**
 * FHTAGN_LITTLE_ENDIAN ntoh()/hton() conversions.
 **/
template <>
struct convert<FHTAGN_LITTLE_ENDIAN>
{
    template <typename intT>
    inline static intT hton(intT const & orig)
    {
        // because network byte order is big endian, we need to swap the
        // endianness of the input integer.
        return swap(orig);
    }

    template <typename intT>
    inline static intT ntoh(intT const & orig)
    {
        // because network byte order is big endian, we need to swap the
        // endianness of the input integer.
        return swap(orig);
    }
};



/**
 * FHTAGN_BIG_ENDIAN ntoh()/hton() conversions.
 **/
template <>
struct convert<FHTAGN_BIG_ENDIAN>
{
    template <typename intT>
    inline static intT hton(intT const & orig)
    {
        // network byte order is already big endian, so no changes required.
        return orig;
    }

    template <typename intT>
    inline static intT ntoh(intT const & orig)
    {
        // network byte order is already big endian, so no changes required.
        return orig;
    }
};


/**
 * Use the to_host() function to convert an integer value from a given endianess
 * to the endianess of the host. The function detects whether endianess swapping
 * needs to be performed or not.
 **/
template <typename intT>
inline intT
to_host(intT const & int_value, endian int_endian)
{
    static bool const decision[2][2] = {
        // int is BE   int is LE
        { false,      true,  }, // host BE
        { true,       false, }, // host LE
    };

    if (decision[FHTAGN_BYTE_ORDER][int_endian]) {
        return swap(int_value);
    }
    return int_value;
}


/**
 * The from_host() function more or less does the same as the to_host() function
 * above, except in reverse - it converts an integer from host byte order to the
 * specified byte order.
 **/
template <typename intT>
inline intT
from_host(intT const & int_value, endian int_endian)
{
    static bool const decision[2][2] = {
        // int is BE   int is LE
        { false,      true,  }, // host BE
        { true,       false, }, // host LE
    };

    if (decision[FHTAGN_BYTE_ORDER][int_endian]) {
        return swap(int_value);
    }
    return int_value;
}

}} // namespace fhtagn::byte_order

#endif // guard
