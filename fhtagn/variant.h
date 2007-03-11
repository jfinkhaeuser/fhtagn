/**
 * Copyright (C) 2007 the authors.
 *
 * Author: Henning Pfeiffer <slashgod@users.sourceforge.net>
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
#ifndef FHTAGN_VARIANT_H
#define FHTAGN_VARIANT_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <map>
#include <vector>
#include <string>
#include <stdexcept>

#include <boost/any.hpp>
#include <boost/static_assert.hpp>
#include <boost/shared_ptr.hpp>

/**
 * Call this macro to specialize the variant for use with the specified type.
 * Must be called in the global namespace.
 **/
#define FHTAGN_VARIANT_SPECIALIZE(_type)                                      \
    namespace fhtagn {                                                        \
        template <>                                                           \
        struct variant::is_specialized<_type>                                 \
        {                                                                     \
        };                                                                    \
    } // namespace

/**
 * Call this macro to specialize the variant for use with the specified
 * value type, but store values internally using the specified holder type.
 * Must be called in the global namespace.
 **/
#define FHTAGN_VARIANT_SPECIALIZE_HOLDER(_value_type, _holder_type)           \
    FHTAGN_VARIANT_SPECIALIZE(_value_type)                                    \
    namespace fhtagn {                                                        \
        template <>                                                           \
        struct variant::specialization_traits<_value_type>                    \
        {                                                                     \
            typedef _holder_type holder_type;                                 \
        };                                                                    \
    } // namespace


namespace fhtagn {

/**
 * The variant class serves the purpose of holding a variety of values, the
 * exact data type of which are determined at run time. The concept is very
 * similar to "any" data types, and in fact variant uses boost::any internally.
 *
 * In contrast to "any", however, you can determine at compile time what data
 * types may be stored in the variant. Assigning a differently typed value to
 * the variant will not compile.
 *
 * Assume you specialized the variant class to store some unsigned integer
 * types:
 *    FHTAGN_VARIANT_SPECIALIZE(uint8_t);
 *    FHTAGN_VARIANT_SPECIALIZE(uint16_t);
 *    FHTAGN_VARIANT_SPECIALIZE(uint32_t);
 *    FHTAGN_VARIANT_SPECIALIZE(uint64_t);
 *
 * The following code would work under these circumstances:
 *    variant x = 12;
 *    uint16_t y = 123;
 *    variant z = y;
 *
 * Assigning a signed integer value or from a signed integer variable, or from
 * a float, would not work:
 *    variant x = -12; // compile error
 *    int16_t y = 123;
 *    variant z = y; // compile error
 *
 *    variant x = 1.0; // compile error
 *
 * These properties make the variant useful for all cases where an "any" data
 * type might provide more flexibility than wanted.
 *
 * Another use-case for a variant comes to mind, however, and that is to write
 * code more closely resembling what weakly-typed langauges, or strongly-typed
 * languages with a lot of automatic type conversion provide:
 *
 *    variant x = 12;
 *    x = std::string("Hello, world!");
 *
 * Both assignments compile just fine, as long as your variant is specialized both
 * for an integer type and std::string.
 *
 * Things become even neater, because this code also works:
 *
 *    variant x;
 *    x["foo"]["bar"] = 12;
 *    x["foo"] = 34;
 *
 * So what happens here?
 *
 * First you should know that in order to emulate the type system that scripting
 * languages commonly supply, a specialization of a variant that holds a map of
 * strings to variants always exist. Another such specialization is one that
 * emulates an array of variants). You can instanciate objects of these special
 * types using the variant::array_t and variant::map_t types respectively.
 *
 * In the above code snippet, we first create a variant named x, which we assign
 * no value. Variants that have never been assigned a value are special, in that
 * they can adopt any type (for which specializations exist) implicitly. It is
 * equally possible to assign such a variant a simle value, as it is to use them
 * as if they were a variant::map_t. More explictly, the example above should
 * have been started like this:
 *
 *    variant x = variant::map_t();
 *    x["foo"] = variant::map_t();
 *    x["foo"]["bar"] = 12;
 *
 * But why go to that effort if you can have it easier?
 *
 * Note that the same trick does not work for variant::array_t - the array_t is
 * modelled after the STL's Sequence concept (it is, in fact, a
 * std::vector<variant>), and thus new elements need to be appended or prepended
 * explicitly. This would work:
 *
 *    variant x = variant::array_t();
 *    x.as<variant::array_t>().push_back(12);
 *
 *    variant z = variant::array_t(1);
 *    z[0] = 12;
 *
 * So what's with that weird syntax above, this as() function call?
 *
 * Variants can't and won't change the fact that C++ is a strongly typed
 * language. There are more reasons to /want/ strongly typed languages (at least
 * in this author's opinion) than there is to want weakly typed languages. All
 * that variant does, therefore, is to save you some of the hassle of casting
 * back and forth between void * and the type you want to use.
 *
 * If you assign an integer value to a variant, the variant will remember that
 * you assigned it an integer value, and you need to extract that integer
 * explictly before you can modify it:
 *
 *    variant x = 12;
 *    x.as<int>() += 3;
 *    assert(x.as<int>() == 15);
 *
 * The as() function requires a template parameter for the type you expect to be
 * stored in the variant, casts it's internal void pointer to the type you
 * specify, and returns a reference to it.
 *
 * If you your variant did not contain an int, however, as() would return a
 * reference to an int at address 0x0 - you would very definitely encounter a
 * segmentation fault.
 *
 * It pays to be sure, therefore, what type your variant contains. You can find
 * that out prior to calling as() by calling is() - is() works in a nearly
 * identical fashion internally, but returns true if the specified type matched
 * the one stored in the variant, and false otherwise:
 *
 *    if (x.is<int>()) {
 *        x.as<int>() += 234; // perfectly safe code.
 *    }
 *
 * And that's how you would normally use a variant. The variant::map_t and
 * variant::array_t types are treated a little more specially, however. The
 * subscript operator for variant that accepts integer values assumes that
 * the held data type is a variant::array_t. Similarily, the subscript
 * operator that accepts strings, assumes that there is a variant::map_t.
 *
 * Because this sort of shortcut must be hardcoded, it's only provided for
 * variant::map_t and variant::array_t - that does not, however, stop you from
 * storing any other container in a variant, it just means that access will be
 * slightly more complex.
 *
 * This variant implementation provides another feature, which makes it
 * particularily useful for storing strings, although the feature can be used
 * for any other data types as well. It might be that you want to write code
 * such as this:
 *
 *    char * foo = "Hello, world!";
 *    variant x = foo;
 *
 * For the sake of the argument, we're assuming that you don't want to store
 * a char * inside the variant. More likely, you want to store the string
 * "Hello, world!" in the variant, that is, you want the variant to behave much
 * like a std::string.
 *
 * In order to try and avoid too many special cases (such as with variant::map_t
 * and variant::array_t), you can instead specialize the variant to use a
 * different data type for the internal holder variable than the data type
 * used in the assignment:
 *
 *    FHTAGN_VARIANT_SPECIALIZE_HOLDER(char *, std::string);
 *    x = foo;
 *    assert(x.as<std::string>() == "Hello, world");
 *
 * Note that all those specialization macros, i.e. FHTAGN_VARIANT_SPECIALIZE
 * and FHTAGN_VARIANT_SPECIALIZE_HOLDER provide compile time checks for
 * assignemnt. That is, if in a given piece of code you assign an integer value
 * to a variant, and somewhere previous to that line (through #includes or
 * directly), no specialization macro for integers appeared, the compile will
 * fail.
 *
 * While that may at first seem cumbersome, it's actually a great feature. It
 * allows you to use variant in several places in your code, and each time you
 * can decide anew which data types you want to support. Ideally, build yourself
 * an internal header file for each use case, and #include that wherever
 * appropriate. The included header file <fhtagn/stdvariant.h> may be a good
 * starting point.
 **/
class variant
{
public:
    /**
     * See class documentation. Specialized map type that maps std::strings to
     * variants.
     **/
    typedef std::map<std::string, variant> map_t;

    /**
     * See class documentation. Specialized array of variants.
     **/
    typedef std::vector<variant> array_t;

    variant();
    ~variant() {}

    /**
     * Variants are CopyConstructible and Assignable from other variants.
     **/
    variant(variant const & other);
    variant & operator=(variant const & other);

    /**
     * Variants are also CopyConstructible and Assignable from any other type -
     * at least in theory. In practice, these functions use
     * specialization_traits<T> internally (see below), which may not be defined
     * for any T. FHTAGN_VARIANT_SPECIALIZE* creates such definitions for you.
     **/
    template <typename T>
    variant(T const & other);
    template <typename T>
    variant & operator=(T const & other);

    /**
     * Part of the variant's specialization magic. If this structure is defined
     * for a valueT, assigning a valueT to the variant will compile... See
     * specialization_traits for details.
     **/
    template <typename valueT>
    struct is_specialized;

    /**
     * Using a traits structure for valueT enables us to do two things:
     * 1. easily define at compile time, whether a different holder_type is to
     *    be used from the valueT specified. By default, holder_type and valueT
     *    are identical.
     * 2. Throw compile time assertions if is_specialized<valueT> is not defined.
     **/
    template <typename valueT>
    struct specialization_traits
    {
        typedef valueT holder_type;
        BOOST_STATIC_ASSERT(sizeof(is_specialized<valueT>) != 0);
    };


    /**
     * Return the held variable in the format (data type) defined by the
     * specialization_traits, as a reference.
     **/
    template <typename T>
    typename specialization_traits<T>::holder_type &
    as();

    /**
     * Return the held variable in the format (data type) defined by the
     * specialization_traits, as a const reference.
     **/
    template <typename T>
    typename specialization_traits<T>::holder_type const &
    as() const;

    /**
     * Returns true if this variant holds a value of type T, else false.
     **/
    template <typename T>
    bool is() const;

    /**
     * Variants may be invalid. Invalid variants can be returned from the
     * subscript operators, if the variant you called the subcript operator
     * on was not subscriptable. If you further use an invalid variant, that
     * will result in a std::logic_error exception. Using this function, you
     * can explicitly check if a variant is invalid.
     **/
    bool is_valid() const;

    /**
     * Assume that the variant holds a map_t or an array_t, and try to access
     * the element specified by the index/key. If the variant does not hold a
     * container, or (depending on the container semantics) the element does
     * not exist, an invalid variant is returned.
     **/
    variant & operator[](uint32_t index);
    variant & operator[](std::string const & key);

    variant const & operator[](uint32_t index) const;
    variant const & operator[](std::string const & key) const;

private:
    /**
     * Internally, variants can be in three states:
     * - Empty variants can be treated as any value, even containers. However,
     *   is() will always return false, and as() will always return a refernce
     *   to an object at 0x0. Once the variant has been assigned a value, or
     *   been treated as a variant::map_t, it's state will be changed to
     *   IS_VALUE.
     * - IS_VALUE specifies variants that have been assigned a value - it does
     *   not matter whether that value is a simple value or a container.
     * - IS_INVALID cannot be constructed explicitly, rather, it's used
     *   internally when the user has tried to perform an illegal operation
     *   on the variant. Using a variant in the IS_INVALID state will result
     *   in a std::logic_error exception.
     **/
    enum variant_state
    {
        IS_EMPTY   = 0,
        IS_VALUE   = 1,
        IS_INVALID = 2,
    };

    /**
     * The default constructor sets the state to IS_EMPTY, copy constructors set
     * the stte to IS_VALUE. In order to construct an invalid value, we need to
     * pass it an explicit state - however, invalid values need never be
     * constructed by the user, therefore this constructor is private.
     **/
    variant(variant_state state);

    /** The state of this variant **/
    variant_state  m_state;
    /** The data (if any) this variant holds **/
    boost::any     m_data;

    /**
     * There is little need creating hundres of invalid variants, where one
     * suffices. Thefore variant has one static variant member that is
     * constructed to be invalid, which is returned whenever one of variant's
     * functions returns an invalid value.
     **/
    static void instanciate_invalid_value();
    static boost::shared_ptr<variant> invalid_value;
};

#include <fhtagn/variant.tcc>

#endif // guard
