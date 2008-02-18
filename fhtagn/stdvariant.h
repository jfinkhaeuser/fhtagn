/**
 * $Id$
 *
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
#ifndef FHTAGN_STDVARIANT_H
#define FHTAGN_STDVARIANT_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <stdint.h>

#include <string>

#include <fhtagn/variant.h>

// specialization for builtin bool type
FHTAGN_VARIANT_SPECIALIZE(bool)

// specializations for builtin integer types
FHTAGN_VARIANT_SPECIALIZE(int8_t)
FHTAGN_VARIANT_SPECIALIZE(uint8_t)
FHTAGN_VARIANT_SPECIALIZE(int16_t)
FHTAGN_VARIANT_SPECIALIZE(uint16_t)
FHTAGN_VARIANT_SPECIALIZE(int32_t)
FHTAGN_VARIANT_SPECIALIZE(uint32_t)
FHTAGN_VARIANT_SPECIALIZE(int64_t)
FHTAGN_VARIANT_SPECIALIZE(uint64_t)

// floating point types
FHTAGN_VARIANT_SPECIALIZE(float)
FHTAGN_VARIANT_SPECIALIZE(double)

// std::string may not be a builtin type, but it's often used
FHTAGN_VARIANT_SPECIALIZE(std::string)
FHTAGN_VARIANT_SPECIALIZE_HOLDER(std::string::value_type *, std::string)
FHTAGN_VARIANT_SPECIALIZE_HOLDER(std::string::value_type const *, std::string)
FHTAGN_VARIANT_SPECIALIZE_HOLDER(std::string::value_type const * const, std::string)

#endif // guard
