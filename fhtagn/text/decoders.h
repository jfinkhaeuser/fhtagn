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
#ifndef FHTAGN_TEXT_DECODERS_H
#define FHTAGN_TEXT_DECODERS_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <fhtagn/fhtagn.h>

#include <fhtagn/byteorder.h>

#include <fhtagn/text/transcoding.h>

namespace fhtagn {
namespace text {

/**
 * The raw_decoder merely transforms 8-bit values to 32-bit values, without
 * placing any restrictions on the representable set of values. As such, it
 * may not produce legal UTF-32 characters.
 *
 * The reason to include a raw decoder is just to allow data to be read
 * verbatim, without being handled any differently from specific character
 * encodings, paving the way for generic programming techniques.
 *
 * If you can avoid using the raw_decoder, though, please do so. As far as
 * space efficiency goes, it's pretty insane.
 **/
struct raw_decoder
    : public transcoder_base
{
    raw_decoder()
        : transcoder_base()
        , m_value(0)
        , m_full(false)
    {
    }

    bool append(unsigned char byte)
    {
        if (have_full_sequence()) {
            return false;
        }

        m_value = byte;
        m_full = true;
        return true;
    }

    bool have_full_sequence() const
    {
        return m_full;
    }

    void reset()
    {
        m_full = false;
    }

    utf32_char_t to_utf32() const
    {
        return m_value;
    }

    utf32_char_t m_value;
    bool         m_full;
};


/**
 * Decodes a byte sequence as ASCII. ASCII only allows 7 bit values, i.e. all
 * values above 127 are considered invalid.
 **/
struct ascii_decoder
    : public transcoder_base
{
    ascii_decoder()
        : transcoder_base()
        , m_byte(128)
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
 * Base decoder for ISO-8859 decoders. Note that the IANA versions of the
 * standard is implemented, i.e. control characters in the C0 and C1 ranges
 * are interpreted as well.
 **/
struct iso8859_decoder_base
    : public transcoder_base
{
    iso8859_decoder_base(fhtagn::size_t subencoding)
        : transcoder_base()
        , m_subencoding(subencoding)
        , m_byte(0)
        , m_empty(true)
    {
    }


    bool append(unsigned char byte)
    {
        if (have_full_sequence()) {
            return false;
        }

        // Characters with values in the range [128, 159] are technically only
        // valid after the 1987/1988/1989 extensions to the ISO 8859 standard.
        m_byte = byte;
        m_empty = false;
        return true;
    }


    bool have_full_sequence() const
    {
        // 128 is an invalid byte value, we use it to signal that the buffer
        // is unused.
        return !m_empty;
    }


    void reset()
    {
        // signal empty buffer
        m_empty = true;
    }


    utf32_char_t to_utf32() const
    {
        if (m_subencoding == 1) {
            // in ISO-8859-1 all characters correspond to the unicode code point
            // of the same value
            return m_byte;
        }

        unsigned char tmp = m_byte;
        if (tmp <= 159) {
            return tmp;
        }

        fhtagn::size_t offset = m_subencoding - 2;
        if (m_subencoding > 11) {
            --offset;
        }
        offset *= 96; // number of characters special to each subencoding
        offset += tmp - 160;
        return detail::iso8859_mapping[offset];
    }

    fhtagn::size_t      m_subencoding;
    unsigned char       m_byte;
    bool                m_empty;
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

FHTAGN_TEXT_DEFINE_ISO8859_DECODER(1)
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(2)
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(3)
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(4)
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(5)
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(6)
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(7)
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(8)
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(9)
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(10)
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(11)
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(13)
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(14)
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(15)
FHTAGN_TEXT_DEFINE_ISO8859_DECODER(16)


/**
 * Decoder for CP 1252. That windows encoding is basically identical to
 * ISO-8859-1, except that the unused range of bytes between 0x80 and 0x9f
 * is mapped to characters, some of which are included in ISO-8859-15.
 **/
struct cp1252_decoder
    : public transcoder_base
{
    cp1252_decoder()
        : transcoder_base()
        , m_byte(0x81) // unused character
    {
    }


    bool append(unsigned char byte)
    {
        if (have_full_sequence()) {
            return false;
        }

        m_byte = byte;
        return true;
    }


    bool have_full_sequence() const
    {
        // 0x81 is an invalid byte value, we use it to signal that the buffer
        // is unused.
        return (m_byte != 0x81);
    }


    void reset()
    {
        // signal empty buffer
        m_byte = 0x81;
    }


    utf32_char_t to_utf32() const
    {
        unsigned char tmp = m_byte;
        if (tmp <= 127 || tmp >= 160) {
            return tmp;
        }
        return detail::cp1252_mapping[tmp - 0x80];
    }

    unsigned char       m_byte;
};



/**
 * Decoder for Mac Roman. This encoding covers all possible 8 bit values.
 **/
struct mac_roman_decoder
    : public transcoder_base
{
    mac_roman_decoder()
        : transcoder_base()
        , m_byte(0)
        , m_empty(true)
    {
    }


    bool append(unsigned char byte)
    {
        if (have_full_sequence()) {
            return false;
        }

        m_byte = byte;
        m_empty = false;
        return true;
    }


    bool have_full_sequence() const
    {
        return !m_empty;
    }


    void reset()
    {
        // signal empty buffer
        m_empty = true;
    }


    utf32_char_t to_utf32() const
    {
        unsigned char tmp = m_byte;
        if (tmp <= 127) {
            return tmp;
        }
        return detail::mac_roman_mapping[tmp - 0x80];
    }

    unsigned char       m_byte;
    bool                m_empty;
};



/**
 * UTF-8 decoder
 **/
struct utf8_decoder
    : public transcoder_base
{
    utf8_decoder()
        : transcoder_base()
        , m_size(0)
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
        for (boost::uint8_t i = 1 ; i < m_size ; ++i) {
            result *= (1 << 6);
            result += static_cast<unsigned char>(m_buffer[i]) - 0x80;
        }

        return result;
    }

    boost::uint8_t  m_size;
    unsigned char   m_buffer[4];
    boost::uint8_t  m_buffer_used;
};



/**
 * UTF-16 decoder
 **/
struct utf16_decoder
    : public transcoder_base
{
    explicit utf16_decoder(byte_order::endian e
            = byte_order::FHTAGN_UNKNOWN_ENDIAN)
        : transcoder_base()
        , m_endian(e)
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

        boost::uint16_t & next_word = m_buffer[m_buffer_used / 2];

        // if we're reading the first byte of either word that can make up a
        // UTF-16 sequence we /always/ need another byte.
        if (m_buffer_used == 0 || m_buffer_used == 2) {
            next_word = (static_cast<boost::uint16_t>(byte) << 8);
            ++m_buffer_used;
            return true;
        }

        // We're building up the next_word contents in a BE fashion; the first
        // byte to come is considered the most significant. That way we can
        // correctly detect the BOM.
        next_word |= byte;
        ++m_buffer_used;

        if (m_endian == byte_order::FHTAGN_UNKNOWN_ENDIAN) {
            switch (next_word) {
                case 0xfffe:
                    // LE BOM
                    m_endian = byte_order::FHTAGN_LITTLE_ENDIAN;
                    reset();
                    return true;
                    break;

                case 0xfeff:
                    // BE BOM
                    m_endian = byte_order::FHTAGN_BIG_ENDIAN;
                    reset();
                    return true;
                    break;

                default:
                    reset();
                    return false;
                    break;
            }
        }

        // Because the buffer is built in a BE fashion, we want to swap the
        // value's endianness if it's meant to be LE.
        if (m_endian == byte_order::FHTAGN_LITTLE_ENDIAN) {
            next_word = byte_order::swap(next_word);
        }

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
    boost::uint16_t     m_buffer[2];

    /** bytes used in m_buffer (not words) */
    boost::uint8_t      m_buffer_used;
    /** word size of the character */
    boost::uint8_t      m_size;
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
    : public transcoder_base
{
    explicit utf32_decoder(byte_order::endian e
            = byte_order::FHTAGN_UNKNOWN_ENDIAN)
        : transcoder_base()
        , m_endian(e)
        , m_buffer(0)
        , m_buffer_used(0)
    {
    }


    void reset()
    {
        m_buffer_used = 0;
        m_buffer = 0;
    }


    bool append(unsigned char byte)
    {
        if (have_full_sequence()) {
            return false;
        }

        // We're building up the m_buffer contents in a BE fashion; the first
        // byte to come is considered the most significant. That way we can
        // correctly detect the BOM.
        m_buffer |= byte << (3 - m_buffer_used++) * 8;

        if (m_buffer_used < 4) {
            return true;
        }

        if (m_endian == byte_order::FHTAGN_UNKNOWN_ENDIAN) {
            switch (m_buffer) {
                case 0xfffe0000:
                    // LE BOM
                    m_endian = byte_order::FHTAGN_LITTLE_ENDIAN;
                    reset();
                    return true;
                    break;

                case 0x0000feff:
                    // BE BOM
                    m_endian = byte_order::FHTAGN_BIG_ENDIAN;
                    reset();
                    return true;
                    break;

                default:
                    reset();
                    return false;
                    break;
            }

        }

        // Because the buffer is built in a BE fashion, we want to swap the
        // value's endianness if it's meant to be LE.
        if (m_endian == byte_order::FHTAGN_LITTLE_ENDIAN) {
            m_buffer = byte_order::swap(m_buffer);
        }

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
    boost::uint32_t     m_buffer;

    /** bytes used in m_buffer (not words) */
    boost::uint8_t      m_buffer_used;
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


/**
 * The universal_decoder follows the same Concept as other decoders, but does
 * not decode one specific encoding. Instead, you can switch the encoding it is
 * meant to decode at run-time. Internally, the universal_decoder delegates to
 * a pimpl'd decoder instance of the required type.
 **/
struct universal_decoder
{
    /** transcoder_base interface **/
    bool use_replacement_char() const;
    void use_replacement_char(bool new_value);

    utf32_char_t replacement_char() const;
    void replacement_char(utf32_char_t new_value);


    /** Standard decoder interface **/
    /**
     * By default, decode ASCII encoding.
     **/
    explicit universal_decoder(char_encoding_type encoding = ASCII);
    ~universal_decoder();

    void reset();

    bool append(unsigned char byte);

    bool have_full_sequence() const;

    utf32_char_t to_utf32() const;

    /**
     * Return the encoding the decoder instance is currently set to decode.
     **/
    char_encoding_type get_encoding() const
    {
        return m_encoding;
    }

    /**
     * Set a new encoding to decoder. This will reset() the decoder.
     **/
    void set_encoding(char_encoding_type new_encoding);

private:
    char_encoding_type  m_encoding;
    void *              m_decoder;
};


}} // namespace fhtagn::text

#endif // guard
