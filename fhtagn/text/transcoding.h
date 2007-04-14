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
#ifndef FHTAGN_TEXT_TRANSCODING_H
#define FHTAGN_TEXT_TRANSCODING_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <string>

#include <iostream> // FIXME

namespace fhtagn {
namespace text {

/**
 * Code in this file is concerned with text in various character encodings.
 * Here's an experpt from the Unicode FAQ, which might serve as a reference for
 * understanding the rest of the code:
 *
 *  Name             UTF-8   UTF-16  UTF-16BE  UTF-16LE   UTF-32  UTF-32BE  UTF-32LE
 *  Min code point    0000     0000      0000      0000     0000      0000      0000
 *  Max code point  10FFFF   10FFFF    10FFFF    10FFFF   10FFFF    10FFFF    10FFFF
 *  Code unit size  8 bits  16 bits   16 bits   16 bits  32 bits   32 bits   32 bits
 *  Byte order         N/A    <BOM>        BE        LE    <BOM>        BE        LE
 *  Min bytes/char       1        2         2         2        4         4         4
 *  Max bytes/char       4        4         4         4        4         4         4
 *
 *  Replacement character: FFFD
 *  BOM: FEFF character converted to the specific encoding, i.e.
 *       00 00 FE FF   UTF-32BE
 *       FF FE 00 00   UTF-32LE
 *       FE FF         UTF-16BE
 *       FF FE         UTF-16LE
 *       EF BB BF      UTF-8
 **/


/**
 * Some basic type definitions. Of course, instead of utf8_string you could just
 * use std::string, but where's the fun in that?
 **/
typedef char      utf8_char_t;
typedef std::basic_string<utf8_char_t>  utf8_string;
typedef uint16_t  utf16_char_t;
typedef std::basic_string<utf16_char_t> utf16_string;
typedef wchar_t   utf32_char_t;
typedef std::basic_string<utf32_char_t> utf32_string;


/**
 * Supported character encoding types by fhtagn's text processing utilities. Not
 * all encoding types must be supported by all parts of the code, but this is
 * enumeration lists the maximum number of supported encodings.
 *
 * TODO CP1252
 **/
enum char_encoding_type
{
    ASCII                 =  0,
    US_ASCII              =  ASCII,
    UTF_8                 =  1,
    UTF_16                =  2,
    UTF_16LE              =  3,
    UTF_16BE              =  4,
    UTF_32                =  5,
    UTF_32LE              =  6,
    UTF_32BE              =  7,
    UCS_4                 =  UTF_32,
    ISO_8859_1            =  8,
    ISO_LATIN_1           =  ISO_8859_1,
    ISO_8859_2            =  9,
    ISO_LATIN_2           =  ISO_8859_2,
    ISO_8859_3            = 10,
    ISO_LATIN_3           = ISO_8859_3,
    ISO_8859_4            = 11,
    ISO_LATIN_4           = ISO_8859_4,
    ISO_8859_5            = 12,
    ISO_LATIN_CYRILLIC    = ISO_8859_5,
    ISO_8859_6            = 13,
    ISO_LATIN_ARABIC      = ISO_8859_6,
    ISO_8859_7            = 14,
    ISO_LATIN_GREEK       = ISO_8859_7,
    ISO_8859_8            = 15,
    ISO_LATIN_HEBREW      = ISO_8859_8,
    ISO_8859_9            = 16,
    ISO_LATIN_5           = ISO_8859_9,
    ISO_8859_10           = 17,
    ISO_LATIN_6           = ISO_8859_10,
    ISO_8859_11           = 18,
    ISO_LATIN_THAI        = ISO_8859_11,
    // Note that work on ISO-5589-12 was started, but never completed.
    ISO_8859_13           = 20,
    ISO_LATIN_7           = ISO_8859_13,
    ISO_8859_14           = 21,
    ISO_LATIN_8           = ISO_8859_14,
    ISO_8859_15           = 22,
    ISO_LATIN_9           = ISO_8859_15,
    ISO_8859_16           = 23,
    ISO_LATIN_10          = ISO_8859_16,
};


/**
 * Base class for decoding byte sequence into a single UTF-32 character. Derived
 * classes determine the way the byte sequence is interpreted, e.g. as ASCII,
 * ISO-8859-1, UTF-8, you name it.
 *
 * In most cases you'll want to use the decode() function below.
 **/
template <
    typename derived_decoderT
>
struct decoder_base
{
    decoder_base()
        : m_buffer_used(0)
    {
    }


    /**
     * Reset decoder to try decoding a new byte sequence.
     **/
    void reset()
    {
        m_buffer_used = 0;
    }


    /**
     * Returns true if the internal byte buffer (filled via the append()
     * function) holds a full sequence of bytes, i.e. a sequence that can be
     * interpreted as a UTF-32 character according to the derived decoder.
     * If the sequence is not complete, false is returned.
     **/
    bool have_full_sequence() const
    {
        return !this->derived().need_more();
    }


    /**
     * Attempts to interpret the internal byte buffer as a UTF-32 character.
     * If have_full_sequence() is false, the results of this call are undefined.
     **/
    utf32_char_t to_utf32() const
    {
        return this->derived().convert_to_utf32();
    }


    /**
     * Append tries to add the provided byte to an internal byte buffer, held in
     * order to decode full byte sequences. If append can add the byte to the
     * buffer (because the buffer does not yet contain a full sequence and the
     * byte is valid according to the derived decoder), append() returns true,
     * else false is returned.
     **/
    bool append(unsigned char byte)
    {
        if (this->derived().have_full_sequence()) {
            return false; // not adding more
        }

        if (this->derived().is_valid(byte)) {
            m_buffer[m_buffer_used++] = byte;
            return true;
        }

        return false;
    }


    /**
     * As the excerpt from the Unicode FAQ above shows, UTF sequences can be at
     * most 4 bytes in length.
     **/
    unsigned char m_buffer[4];

    /**
     * Number of bytes in the buffer currently in use.
     **/
    uint8_t m_buffer_used;

    derived_decoderT & derived()
    {
        return *static_cast<derived_decoderT *>(this);
    }


    derived_decoderT const & derived() const
    {
        return *static_cast<derived_decoderT const *>(this);
    }
};



/**
 * The decode() function must be parametrized with a concrete byte decoder,
 * i.e. a decoder derived from decoder_base<> above. Other than that, the usage
 * of the function is similar to standard algorithms such as std::copy:
 *
 * decode<decoder>(input.begin(), input.end(), output_iterator, {true|false});
 *
 * The function accepts a fourth, optional, non-standard parameter specifying
 * whether it should produce replacement characters (0xfffd, see above) when
 * encountering invalid bytes in the input sequence, or stop decoding. The
 * default (true) is to produce replacement characters.
 *
 * The function returns the iterator one past the last byte in the input
 * sequence it could decode - in normal cases, that'll be the second parameter,
 * but when replacement characters are not meant to be produced, it'll point
 * to the first invalid character encountered.
 **/
template <
    typename decoderT,
    typename input_iterT,
    typename output_iterT
>
inline input_iterT
decode(input_iterT first, input_iterT last, output_iterT result,
    bool use_replacement_char = true)
{
    decoderT decoder;

    input_iterT iter = first;
    for ( ; iter != last ; ++iter) {
        if (decoder.have_full_sequence()) {
            // if we have a full sequence, we can decode it to utf32, and
            // restart.
            *result++ = decoder.to_utf32();
            decoder.reset();
        }

        if (!decoder.append(*iter)) {
            // *iter must've been invalid - we checked before whether appending
            // can fail because we have a full sequence, and if that's the case
            // the decoder is reset.
            if (use_replacement_char) {
                // if the caller wants us to produce replacement chars for
                // anything we can't decode, let's do that...
                *result++ = 0xfffd;
                decoder.reset();
            } else {
                // ... otherwise we bail out, providing the iterator that
                // proved troublesome
                return iter;
            }
        }
    }

    if (decoder.have_full_sequence()) {
        // the decoder might still have a full sequence
        *result++ = decoder.to_utf32();
    }

    return iter;
}




}} // namespace fhtagn::text

#endif // guard
