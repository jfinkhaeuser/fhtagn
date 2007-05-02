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
#ifndef FHTAGN_TEXT_ENCODERS_H
#define FHTAGN_TEXT_ENCODERS_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <fhtagn/text/transcoding.h>

namespace fhtagn {
namespace text {

/**
 * Encodes a UTF-32 character into a single ASCII byte. ASCII allows only 7 bit
 * values, i.e. all values above 127 are considered invalid.
 **/
struct ascii_encoder
{
    typedef char const * const_iterator;

    const_iterator begin() const
    {
        // If m_byte has a valid value, return m_byte's address. Else return
        // end()
        return &m_byte + (m_byte == -1 ? 1 : 0);
    }

    const_iterator end() const
    {
        return &m_byte + 1;
    }

    bool encode(utf32_char_t ch)
    {
        // Only characters with values <= 127 can be encoded in ASCII - higher
        // value characters require a different encoding. Note that this also
        // implies that the default replacement character 0xfffd cannot be
        // encoded in ASCII.
        if (0 <= ch && ch <= 127) {
            m_byte = static_cast<char>(ch);
            return true;
        }
        m_byte = -1; // signal empty buffer
        return false;
    }

    char m_byte;
};



/**
 * TODO
 **/
struct iso8859_encoder_base
{
    typedef char const * const_iterator;

    iso8859_encoder_base(uint32_t subencoding)
        : m_subencoding(subencoding)
        , m_byte(-1)
    {
    }

    const_iterator begin() const
    {
        // If m_byte has a valid value, return m_byte's address. Else return
        // end()
        return &m_byte + (m_byte == -1 ? 1 : 0);
    }

    const_iterator end() const
    {
        return &m_byte + 1;
    }

    bool encode(utf32_char_t ch)
    {
        // Characters with values <= 127 can be encoded in just like in ASCII
        if (0 <= ch && ch <= 127) {
            m_byte = static_cast<char>(ch);
            return true;
        }

        if (m_subencoding == 1) {
            // in iso-8859-1, all characters correspond to unicode code points
            // of the same value.
            if (ch <= 255) {
                m_byte = static_cast<char>(ch);
                return true;
            }
        } else {
            // compute offset into mapping table
            uint32_t offset = m_subencoding - 2;
            if (m_subencoding > 11) {
                --offset;
            }
            offset *= 96; // number of characters special to each subencoding

            for (uint32_t i = offset ; i < offset + 96 ; ++i) {
                if (detail::iso8859_mapping[i] == ch) {
                    m_byte = i - offset + 160;
                    return true;
                }
            }
        }

        // other bytes can't be encoded in any of the iso8859 encodings.
        m_byte = -1; // signal empty buffer
        return false;
    }

    uint32_t  m_subencoding;
    char      m_byte;
};


#define FHTAGN_TEXT_DEFINE_ISO8859_ENCODER(subencoding)       \
    struct iso8859_##subencoding##_encoder                    \
        : public iso8859_encoder_base                         \
    {                                                         \
        iso8859_##subencoding##_encoder()                     \
            : iso8859_encoder_base(subencoding)               \
        {                                                     \
        }                                                     \
    };

FHTAGN_TEXT_DEFINE_ISO8859_ENCODER(1);
FHTAGN_TEXT_DEFINE_ISO8859_ENCODER(2);
FHTAGN_TEXT_DEFINE_ISO8859_ENCODER(3);
FHTAGN_TEXT_DEFINE_ISO8859_ENCODER(4);
FHTAGN_TEXT_DEFINE_ISO8859_ENCODER(5);
FHTAGN_TEXT_DEFINE_ISO8859_ENCODER(6);
FHTAGN_TEXT_DEFINE_ISO8859_ENCODER(7);
FHTAGN_TEXT_DEFINE_ISO8859_ENCODER(8);
FHTAGN_TEXT_DEFINE_ISO8859_ENCODER(9);
FHTAGN_TEXT_DEFINE_ISO8859_ENCODER(11);
FHTAGN_TEXT_DEFINE_ISO8859_ENCODER(13);
FHTAGN_TEXT_DEFINE_ISO8859_ENCODER(14);
FHTAGN_TEXT_DEFINE_ISO8859_ENCODER(15);
FHTAGN_TEXT_DEFINE_ISO8859_ENCODER(16);




}} // namespace fhtagn::text

#endif //guard
