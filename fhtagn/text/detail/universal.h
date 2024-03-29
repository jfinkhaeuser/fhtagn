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
#ifndef FHTAGN_TEXT_DETAIL_UNIVERSAL_H
#define FHTAGN_TEXT_DETAIL_UNIVERSAL_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <fhtagn/fhtagn.h>

#include <assert.h>
#include <fhtagn/text/transcoding.h>
#include <fhtagn/text/decoders.h>
#include <fhtagn/text/encoders.h>

/**
 * Delegate a function call to an encoder or decoder instance. The
 * run-time specified encoding is provided as the first parameter,
 * the (void *) encoder/decoder instance as the second. Function name
 * and argument list make up the third and fourth parameter.
 *
 * The type to cast to from the void * to the specific encoder/decoder
 * instance is deduced from the static mapping (see below). This macro
 * is used to implement the FHTAGN_TEXT_CALL macro below.
 **/
#define FHTAGN_TEXT_CALL_CASE(ENCODING, TRANSCODER, SUBTYPE, FUNC, ARGLIST) \
    case ENCODING:                                                    \
      return static_cast<                                             \
              detail::universal_transcoding_static_mapping<           \
                  ENCODING                                            \
              >::types:: SUBTYPE##_type *>(TRANSCODER)->FUNC ARGLIST; \
      break;

/**
 * Call the given function on the given object, providing the argument
 * list. The object is expected to be a void *, to be cast to a specific
 * encoder/decoder instance. What type to cast to is determined at run-time
 * via the first parameter.
 **/
#define FHTAGN_TEXT_CALL(SWITCHVAR, OBJECT, SUBTYPE, FUNC, ARGLIST)         \
    switch (SWITCHVAR) {                                                    \
        FHTAGN_TEXT_CALL_CASE(RAW,         OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(ASCII,       OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(UTF_8,       OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(UTF_16,      OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(UTF_16LE,    OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(UTF_16BE,    OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(UTF_32,      OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(UTF_32LE,    OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(UTF_32BE,    OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(ISO_8859_1,  OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(ISO_8859_2,  OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(ISO_8859_3,  OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(ISO_8859_4,  OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(ISO_8859_5,  OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(ISO_8859_6,  OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(ISO_8859_7,  OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(ISO_8859_8,  OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(ISO_8859_9,  OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(ISO_8859_10, OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(ISO_8859_11, OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(ISO_8859_13, OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(ISO_8859_14, OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(ISO_8859_15, OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(ISO_8859_16, OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(CP_1252,     OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        FHTAGN_TEXT_CALL_CASE(MAC_ROMAN,   OBJECT, SUBTYPE, FUNC, ARGLIST)  \
        default:                                                            \
            assert(0);                                                      \
            break;                                                          \
    }


/**
 * Create an encoder or decoder instance suitable to the ENCODING, and
 * store it in OBJECT.
 **/
#define FHTAGN_TEXT_CREATE_TYPE(ENCODING, OBJECT, SUBTYPE)            \
    case ENCODING:                                                    \
        OBJECT = new detail::universal_transcoding_static_mapping<    \
                ENCODING                                              \
            >::types:: SUBTYPE##_type();                              \
        break;

/**
 * Using FHTAGN_TEXT_CREATE_TYPE above, create an encoder or decoder
 * based on the value of SWITCHVAR.
 **/
#define FHTAGN_TEXT_CREATE(SWITCHVAR, OBJECT, SUBTYPE)                \
    switch (SWITCHVAR) {                                              \
        FHTAGN_TEXT_CREATE_TYPE(RAW,          OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(ASCII,        OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(UTF_8,        OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(UTF_16,       OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(UTF_16LE,     OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(UTF_16BE,     OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(UTF_32,       OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(UTF_32LE,     OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(UTF_32BE,     OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(ISO_8859_1,   OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(ISO_8859_2,   OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(ISO_8859_3,   OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(ISO_8859_4,   OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(ISO_8859_5,   OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(ISO_8859_6,   OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(ISO_8859_7,   OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(ISO_8859_8,   OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(ISO_8859_9,   OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(ISO_8859_10,  OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(ISO_8859_11,  OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(ISO_8859_13,  OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(ISO_8859_14,  OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(ISO_8859_15,  OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(ISO_8859_16,  OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(CP_1252,      OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_CREATE_TYPE(MAC_ROMAN,    OBJECT, SUBTYPE)        \
        default:                                                      \
            assert(0);                                                \
            break;                                                    \
    }

/**
 * Destroy an encoder or decoder instance stored in OBJECT, casting it
 * to a type suitable to the ENCODING.
 **/
#define FHTAGN_TEXT_DESTROY_TYPE(ENCODING, OBJECT, SUBTYPE)           \
    case ENCODING:                                                    \
        delete static_cast<                                           \
              detail::universal_transcoding_static_mapping<           \
                  ENCODING                                            \
              >::types:: SUBTYPE##_type *>(OBJECT);                   \
        break;


/**
 * Using FHTAGN_TEXT_DESTROY_TYPE above, destroy an encoder or decoder
 * based on the value of SWITCHVAR.
 **/
#define FHTAGN_TEXT_DESTROY(SWITCHVAR, OBJECT, SUBTYPE)               \
    switch (SWITCHVAR) {                                              \
        FHTAGN_TEXT_DESTROY_TYPE(RAW,         OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(ASCII,       OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(UTF_8,       OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(UTF_16,      OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(UTF_16LE,    OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(UTF_16BE,    OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(UTF_32,      OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(UTF_32LE,    OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(UTF_32BE,    OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(ISO_8859_1,  OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(ISO_8859_2,  OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(ISO_8859_3,  OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(ISO_8859_4,  OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(ISO_8859_5,  OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(ISO_8859_6,  OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(ISO_8859_7,  OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(ISO_8859_8,  OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(ISO_8859_9,  OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(ISO_8859_10, OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(ISO_8859_11, OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(ISO_8859_13, OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(ISO_8859_14, OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(ISO_8859_15, OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(ISO_8859_16, OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(CP_1252,     OBJECT, SUBTYPE)        \
        FHTAGN_TEXT_DESTROY_TYPE(MAC_ROMAN,   OBJECT, SUBTYPE)        \
        default:                                                      \
            assert(0);                                                \
            break;                                                    \
    }


/**
 * Map the specified ENCODING at compile time to the given DECODER and
 * ENCODER types.
 **/
#define FHTAGN_TEXT_STATIC_MAP(ENCODING, DECODER, ENCODER)            \
    template <>                                                       \
    struct universal_transcoding_static_mapping<ENCODING>             \
    {                                                                 \
        typedef universal_transcoding_mapping_type<                   \
            DECODER,                                                  \
            ENCODER                                                   \
        > types;                                                      \
    };



namespace fhtagn {
namespace text {
namespace detail {

/**
 * "Value" type for a compile-time mapping of encoding types to appropriate
 * encoder and decoder types. For each enum value of possible encodings, we want
 * one decoder_type and on encoder_type.
 **/
template <
    typename decoderT,
    typename encoderT
>
struct universal_transcoding_mapping_type
{
    typedef decoderT decoder_type;
    typedef encoderT encoder_type;
};


/**
 * Implements the mapping of encoding type to the above decoder + encoder type.
 * The default is to not define those types; however, using the
 * FHTAGN_TEXT_STATIC_MAP macro below, one can create such mappings.
 **/
template <
    unsigned int MAPPING_INDEX
>
struct universal_transcoding_static_mapping
{
};


FHTAGN_TEXT_STATIC_MAP(RAW,         raw_decoder,        raw_encoder)
FHTAGN_TEXT_STATIC_MAP(ASCII,       ascii_decoder,      ascii_encoder)
FHTAGN_TEXT_STATIC_MAP(UTF_8,       utf8_decoder,       utf8_encoder)
FHTAGN_TEXT_STATIC_MAP(UTF_16,      utf16_decoder,      utf16_encoder)
FHTAGN_TEXT_STATIC_MAP(UTF_16LE,    utf16le_decoder,    utf16le_encoder)
FHTAGN_TEXT_STATIC_MAP(UTF_16BE,    utf16be_decoder,    utf16be_encoder)
FHTAGN_TEXT_STATIC_MAP(UTF_32,      utf32_decoder,      utf32_encoder)
FHTAGN_TEXT_STATIC_MAP(UTF_32LE,    utf32le_decoder,    utf32le_encoder)
FHTAGN_TEXT_STATIC_MAP(UTF_32BE,    utf32be_decoder,    utf32be_encoder)
FHTAGN_TEXT_STATIC_MAP(ISO_8859_1,  iso8859_1_decoder,  iso8859_1_encoder)
FHTAGN_TEXT_STATIC_MAP(ISO_8859_2,  iso8859_2_decoder,  iso8859_2_encoder)
FHTAGN_TEXT_STATIC_MAP(ISO_8859_3,  iso8859_3_decoder,  iso8859_3_encoder)
FHTAGN_TEXT_STATIC_MAP(ISO_8859_4,  iso8859_4_decoder,  iso8859_4_encoder)
FHTAGN_TEXT_STATIC_MAP(ISO_8859_5,  iso8859_5_decoder,  iso8859_5_encoder)
FHTAGN_TEXT_STATIC_MAP(ISO_8859_6,  iso8859_6_decoder,  iso8859_6_encoder)
FHTAGN_TEXT_STATIC_MAP(ISO_8859_7,  iso8859_7_decoder,  iso8859_7_encoder)
FHTAGN_TEXT_STATIC_MAP(ISO_8859_8,  iso8859_8_decoder,  iso8859_8_encoder)
FHTAGN_TEXT_STATIC_MAP(ISO_8859_9,  iso8859_9_decoder,  iso8859_9_encoder)
FHTAGN_TEXT_STATIC_MAP(ISO_8859_10, iso8859_10_decoder, iso8859_10_encoder)
FHTAGN_TEXT_STATIC_MAP(ISO_8859_11, iso8859_11_decoder, iso8859_11_encoder)
FHTAGN_TEXT_STATIC_MAP(ISO_8859_13, iso8859_13_decoder, iso8859_13_encoder)
FHTAGN_TEXT_STATIC_MAP(ISO_8859_14, iso8859_14_decoder, iso8859_14_encoder)
FHTAGN_TEXT_STATIC_MAP(ISO_8859_15, iso8859_15_decoder, iso8859_15_encoder)
FHTAGN_TEXT_STATIC_MAP(ISO_8859_16, iso8859_16_decoder, iso8859_16_encoder)
FHTAGN_TEXT_STATIC_MAP(CP_1252,     cp1252_decoder,     cp1252_encoder)
FHTAGN_TEXT_STATIC_MAP(MAC_ROMAN,   mac_roman_decoder,  mac_roman_encoder)
//FHTAGN_TEXT_STATIC_MAP(,     _decoder,    _encoder)


}}} // namespace fhtagn::text::detail

#endif // guard
