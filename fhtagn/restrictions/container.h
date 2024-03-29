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
#ifndef FHTAGN_DETAIL_CONTAINER_RESTRICTIONS_H
#define FHTAGN_DETAIL_CONTAINER_RESTRICTIONS_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <fhtagn/fhtagn.h>

namespace fhtagn {
namespace restrictions {
namespace container {

/**
 * Enforces non-empty container types, where any valueT that has an empty()
 * function that returns true when the value is empty is considered a container
 * type.
 **/
template <
    typename valueT,
    typename next_restrictionT = none<valueT>
>
struct non_empty
{
    static inline valueT const & check(valueT const & value)
    {
        boost::function_requires<concepts::RestrictionConcept<valueT, next_restrictionT> >();

        if (value.empty()) {
            throw violation_error("fhtagn::restrictions::container::non_empty failed!");
        }
        return next_restrictionT::check(value);
    }
};


/**
 * Enforces empty container types, where any valueT that has an empty() function
 * that returns true when the value is empty is considered a container type.
 **/
template <
    typename valueT,
    typename next_restrictionT = none<valueT>
>
struct empty
{
    static inline valueT const & check(valueT const & value)
    {
        boost::function_requires<concepts::RestrictionConcept<valueT, next_restrictionT> >();

        if (!value.empty()) {
            throw violation_error("fhtagn::restrictions::container::empty failed!");
        }
        return next_restrictionT::check(value);
    }
};


}}} // namespace fhtagn::restrictions::container

#endif // guard
