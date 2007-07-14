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

#include <fhtagn/text/transcoding.h>
#include <fhtagn/text/detail/universal.h>


namespace fhtagn {
namespace text {


universal_encoder::universal_encoder(char_encoding_type encoding /* = ASCII */)
    : m_encoding(encoding)
    , m_encoder(0)
{
    set_encoding(encoding);
}


universal_encoder::~universal_encoder()
{
    FHTAGN_TEXT_DESTROY(m_encoding, m_encoder, encoder);
}



void
universal_encoder::set_encoding(char_encoding_type new_encoding)
{
    FHTAGN_TEXT_DESTROY(m_encoding, m_encoder, encoder);
    m_encoding = new_encoding;
    FHTAGN_TEXT_CREATE(m_encoding, m_encoder, encoder);
}



universal_encoder::const_iterator
universal_encoder::begin() const
{
    FHTAGN_TEXT_CALL(m_encoding, m_encoder, encoder, begin, ());
}


universal_encoder::const_iterator
universal_encoder::end() const
{
    FHTAGN_TEXT_CALL(m_encoding, m_encoder, encoder, end, ());
}


bool
universal_encoder::encode(utf32_char_t ch)
{
    FHTAGN_TEXT_CALL(m_encoding, m_encoder, encoder, encode, (ch));
}


bool
universal_encoder::use_replacement_char() const
{
    FHTAGN_TEXT_CALL(m_encoding, m_encoder, encoder, use_replacement_char, ());
}


void
universal_encoder::use_replacement_char(bool new_value)
{
    FHTAGN_TEXT_CALL(m_encoding, m_encoder, encoder, use_replacement_char,
            (new_value));
}


utf32_char_t
universal_encoder::replacement_char() const
{
    FHTAGN_TEXT_CALL(m_encoding, m_encoder, encoder, replacement_char, ());
}


void
universal_encoder::replacement_char(utf32_char_t new_value)
{
    FHTAGN_TEXT_CALL(m_encoding, m_encoder, encoder, replacement_char,
            (new_value));
}



}} // namespace fhtagn::text
