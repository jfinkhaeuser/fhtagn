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
#ifndef FHTAGN_TEXT_DECODERS_H
#define FHTAGN_TEXT_DECODERS_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <fhtagn/byteorder.h>

#include <fhtagn/text/transcoding.h>

namespace fhtagn {
namespace text {

/**
 * Decodes a byte sequence as ASCII. ASCII only allows 7 bit values, i.e. all
 * values above 127 are considered invalid.
 **/
struct ascii_decoder
{
    ascii_decoder()
        : m_byte(128)
    {
    }

    bool append(unsigned char byte)
    {
        if (have_full_sequence()) {
            return false;
        }

        if (byte <= 127) {
            m_byte = byte;
            return true;
        }

        return false;
    }

    bool have_full_sequence() const
    {
        // use invalid char value 128 to signal an empty buffer
        return (m_byte != 128);
    }

    void reset()
    {
        // signal empty buffer
        m_byte = 128;
    }

    utf32_char_t to_utf32() const
    {
        utf32_char_t retval = m_byte;
        return retval;
    }

    unsigned char m_byte;
};


/**
 * Base decoder for ISO-8859 decoders. ISO-8859 includes ASCII and bytes with a
 * value between 160 and 255, but the in between block of bytes is left undefined.
 **/
struct iso8859_decoder_base
{
    iso8859_decoder_base(uint32_t subencoding)
        : m_subencoding(subencoding)
        , m_byte(128)
    {
    }


    bool append(unsigned char byte)
    {
        if (have_full_sequence()) {
            return false;
        }

        if (byte <= 127 || byte >= 160) {
            m_byte = byte;
            return true;
        }

        return false;
    }


    bool have_full_sequence() const
    {
        // 128 is an invalid byte value, we use it to signal that the buffer
        // is unused.
        return (m_byte != 128);
    }


    void reset()
    {
        // signal empty buffer
        m_byte = 128;
    }


    utf32_char_t to_utf32() const
    {
        if (m_subencoding == 1) {
            // in ISO-8859-1 all characters correspond to the unicode code point
            // of the same value
            return m_byte;
        }

        unsigned char tmp = m_byte;
        if (tmp <= 127) {
            return tmp;
        }

        uint32_t offset = m_subencoding - 2;
        if (m_subencoding > 11) {
            --offset;
        }
        offset *= 96; // number of characters special to each subencoding
        offset += tmp - 160;
        return detail::iso8859_mapping[offset];
    }

    uint32_t            m_subencoding;
    unsigned char       m_byte;
};


#define FHTAGN_TEXT_DEFINE_ISO8859_DECODER(subencoding)       \
    struct iso8859_##subencoding##_decoder                    \
        : public iso8859_decoder_base                         \
    {                                                         \
        iso8859_##subencoding##_decoder()                     \
            : iso8859_decoder_base(subencoding)               \
        {                                                     \
        }                                                     \
    };

FHTAGN_TEXT_DEFINE_ISO8859_DECODER(1);
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(2);
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(3);
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(4);
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(5);
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(6);
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(7);
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(8);
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(9);
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(11);
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(13);
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(14);
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(15);
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(16);


/**
 * UTF-8 decoder
 **/
struct utf8_decoder
{
    utf8_decoder()
        : m_size(0)
        , m_buffer_used(0)
    {
    }


    void reset()
    {
        m_size = 0;
        m_buffer_used = 0;
    }


    bool append(unsigned char byte)
    {
        if (have_full_sequence()) {
            return false;
        }

        bool valid = false;
        if (m_size == 0) {
            valid = is_valid_leading_octet(byte);
        } else {
            valid = is_valid_continuing_octet(byte);
        }

        if (valid) {
            m_buffer[m_buffer_used++] = byte;
        }

        return valid;
    }


    bool is_valid_leading_octet(unsigned char byte)
    {
        if (byte < 0x7f) {
            m_size = 1;
            return true;
        }
        if (0xc0 <= byte && byte <= 0xfd) {
            if (0xc0 <= byte && byte <= 0xdf) m_size = 2;
            else if (0xe0 <= byte && byte <= 0xef) m_size = 3;
            else if (0xf0 <= byte && byte <= 0xf7) m_size = 4;
            else if (0xf8 <= byte && byte <= 0xfb) m_size = 5;
            else m_size = 6;
            // According to the standard, UTF-8 characters may be encoded in no
            // more than 4 bytes. One can encode values up to 6 bytes in length
            // in UTF-8, however.
            return (m_size <= 4);
        }
        return false;
    }


    bool is_valid_continuing_octet(unsigned char byte)
    {
        if (byte < 0x80|| 0xbf< byte) {
            return false;
        }
        return true;
    }


    bool have_full_sequence() const
    {
        return !(m_size == 0 || m_buffer_used < m_size);
    }


    utf32_char_t to_utf32() const
    {
        static utf32_char_t const modifier_table[] = {
            0x00, 0xc0, 0xe0, 0xf0,
        };

        utf32_char_t result = static_cast<unsigned char>(m_buffer[0])
                - modifier_table[m_size - 1];
        for (uint8_t i = 1 ; i < m_size ; ++i) {
            result *= (1 << 6);
            result += static_cast<unsigned char>(m_buffer[i]) - 0x80;
        }

        return result;
    }

    uint8_t       m_size;
    unsigned char m_buffer[4];
    uint8_t       m_buffer_used;
};



/**
 * UTF-16 decoder
 **/
struct utf16_decoder
{
    explicit utf16_decoder(byte_order::endian e
            = byte_order::FHTAGN_UNKNOWN_ENDIAN)
        : m_endian(e)
        , m_buffer_used(0)
        , m_size(0)
    {
    }


    void reset()
    {
        m_size = 0;
        m_buffer_used = 0;
    }


    bool append(unsigned char byte)
    {
        if (have_full_sequence()) {
            return false;
        }

        uint16_t & next_word = m_buffer[m_buffer_used / 2];

        // if we're reading the first byte of either word that can make up a
        // UTF-16 sequence we /always/ need another byte.
        if (m_buffer_used == 0 || m_buffer_used == 2) {
            next_word = byte;
            ++m_buffer_used;
            return true;
        }

        next_word += (byte << 8);
        ++m_buffer_used;

        if (m_endian == byte_order::FHTAGN_UNKNOWN_ENDIAN) {
            switch (next_word) {
                case 0xfffe:
                    // LE BOM, but due to the way we assemble words it means the
                    // input is BE.
                    m_endian = byte_order::FHTAGN_BIG_ENDIAN;
                    reset();
                    return true;
                    break;

                case 0xfeff:
                    // BE BOM, but due to the way we assemble words it means the
                    // input is LE.
                    m_endian = byte_order::FHTAGN_LITTLE_ENDIAN;
                    reset();
                    return true;
                    break;

                default:
                    reset();
                    return false;
                    break;
            }
        }

        next_word = byte_order::to_host(next_word, m_endian);

        // if m_next_word is the leading word...
        if (m_size == 0) {
            m_size = 2;
            // ... it might signal that there's a following word ...
            if (0xd800UL < next_word && next_word <= 0xdbffUL) {
                m_size += 2;
                return true;
            } else
            // ... or it might be invalid.
            if (0xdc00UL < next_word && next_word <= 0xdfffUL) {
                reset();
                return false;
            }
            return true;
        }

        // if m_next_word is a second word, it may not be in a valid range.
        if (!(0xdc00UL < next_word && next_word <= 0xdfffUL)) {
            reset();
            return false;
        }

        return true;
    }


    bool have_full_sequence() const
    {
        return !(m_size == 0 || m_buffer_used < m_size);
    }


    utf32_char_t to_utf32() const
    {
        utf32_char_t result = m_buffer[0];
        if (m_size == 4) {
            result = ((result - 0xd800UL) << 10)
                + (m_buffer[1] - 0xdc00UL) + 0x0010000UL;
        }

        return result;
    }

    byte_order::endian  m_endian;
    uint16_t            m_buffer[2];

    /** bytes used in m_buffer (not words) */
    uint8_t             m_buffer_used;
    /** word size of the character */
    uint8_t             m_size;
};


struct utf16le_decoder
    : public utf16_decoder
{
    utf16le_decoder()
        : utf16_decoder(byte_order::FHTAGN_LITTLE_ENDIAN)
    {
    }
};


struct utf16be_decoder
    : public utf16_decoder
{
    utf16be_decoder()
        : utf16_decoder(byte_order::FHTAGN_BIG_ENDIAN)
    {
    }
};


/**
 * UTF-32 decoder
 **/
struct utf32_decoder
{
    explicit utf32_decoder(byte_order::endian e
            = byte_order::FHTAGN_UNKNOWN_ENDIAN)
        : m_endian(e)
        , m_buffer_used(0)
    {
    }


    void reset()
    {
        m_buffer_used = 0;
    }


    bool append(unsigned char byte)
    {
        if (have_full_sequence()) {
            return false;
        }

        unsigned char * buffer = reinterpret_cast<unsigned char *>(&m_buffer);
        buffer[m_buffer_used++] = byte;

        if (m_buffer_used < 4) {
            return true;
        }

        if (m_endian == byte_order::FHTAGN_UNKNOWN_ENDIAN) {
            switch (m_buffer) {
                case 0xfffe0000:
                    // LE BOM, but due to the way we assemble the buffer it
                    // means the input is BE.
                    m_endian = byte_order::FHTAGN_BIG_ENDIAN;
                    reset();
                    return true;
                    break;

                case 0x0000feff:
                    // BE BOM, but due to the way we assemble the buffer it
                    // means the input is LE.
                    m_endian = byte_order::FHTAGN_LITTLE_ENDIAN;
                    reset();
                    return true;
                    break;

                default:
                    reset();
                    return false;
                    break;
            }

        }

        m_buffer = byte_order::to_host(m_buffer, m_endian);
        return true;
    }


    bool have_full_sequence() const
    {
        return (m_buffer_used == 4);
    }


    utf32_char_t to_utf32() const
    {
        return m_buffer;
    }


    byte_order::endian  m_endian;
    uint32_t            m_buffer;

    /** bytes used in m_buffer (not words) */
    uint8_t             m_buffer_used;
};


struct utf32le_decoder
    : public utf32_decoder
{
    utf32le_decoder()
        : utf32_decoder(byte_order::FHTAGN_LITTLE_ENDIAN)
    {
    }
};


struct utf32be_decoder
    : public utf32_decoder
{
    utf32be_decoder()
        : utf32_decoder(byte_order::FHTAGN_BIG_ENDIAN)
    {
    }
};




}} // namespace fhtagn::text

#endif // guard
