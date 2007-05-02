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

#include <cppunit/extensions/HelperMacros.h>

#include <fhtagn/text/transcoding.h>
#include <fhtagn/text/decoders.h>
#include <fhtagn/text/encoders.h>
// #include <fhtagn/text/xparser.h>

namespace {


} // anonymous namespace

class TextTest
    : public CppUnit::TestFixture
{
public:
    CPPUNIT_TEST_SUITE(TextTest);

        CPPUNIT_TEST(testDecodeASCII);
        CPPUNIT_TEST(testDecodeISO_8859_15);
        CPPUNIT_TEST(testDecodeUTF_8);
        CPPUNIT_TEST(testDecodeUTF_16);
        CPPUNIT_TEST(testDecodeUTF_32);

        CPPUNIT_TEST(testEncodeASCII);
        CPPUNIT_TEST(testEncodeISO_8859_15);

    CPPUNIT_TEST_SUITE_END();
private:

    void testDecodeASCII()
    {
        namespace t = fhtagn::text;
        t::ascii_decoder dec;

        // decode an ASCII string
        {
            std::string source = "Hello, world!";
            t::utf32_char_t expected_array[] = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!', '\0' };
            t::utf32_string expected = expected_array;
            t::utf32_string target;
            t::decode<t::ascii_decoder>(source.begin(), source.end(),
                    std::back_insert_iterator<t::utf32_string>(target));
            CPPUNIT_ASSERT_EQUAL(expected.size(), target.size());
            // can't use CPPUNIT_ASSERT_EQUAL because no streaming operator is
            // defined for utf32_strings.
            CPPUNIT_ASSERT(expected == target);
        }

        // test decoding into a static utf32_char_t array, i.e. non-string
        // targets
        {
            std::string source = "Hello, world!";
            t::utf32_char_t target[source.size()];
            t::decode<t::ascii_decoder>(source.begin(), source.end(), target);
         }

        // ensure that an extended ASCII string will be decoded with replacement
        // chars...
        {
            std::string source = "Hello, \xf3 world!";
            t::utf32_string target;
            std::string::iterator error_iter = t::decode<t::ascii_decoder>(
                    source.begin(), source.end(),
                    std::back_insert_iterator<t::utf32_string>(target));
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), target.size());
            CPPUNIT_ASSERT_EQUAL(static_cast<std::string::difference_type>(15),
                    (error_iter - source.begin()));
            // ensure that instead of the invalid 0xf3, a replacement char
            // is produced
            CPPUNIT_ASSERT_EQUAL(static_cast<t::utf32_char_t>(0xfffd), target[7]);
        }

        // ... or decoding ends if replacement chars are not desired
        {
            std::string source = "Hello, \xf3 world!";
            t::utf32_string target;
            std::string::iterator error_iter = t::decode<t::ascii_decoder>(
                    source.begin(), source.end(),
                    std::back_insert_iterator<t::utf32_string>(target), false);
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), target.size());
            CPPUNIT_ASSERT_EQUAL(static_cast<std::string::difference_type>(7),
                    (error_iter - source.begin()));
        }
    }


    void testDecodeISO_8859_15()
    {
        namespace t = fhtagn::text;

        // \xa4 is the euro sign in ISO-8859-15
        std::string source = "Hello, \xa4 world!";
        t::utf32_string target;
        std::string::iterator error_iter = t::decode<t::iso8859_15_decoder>(
                source.begin(), source.end(),
                std::back_insert_iterator<t::utf32_string>(target));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), target.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::string::difference_type>(15),
                (error_iter - source.begin()));
        // ensure that the euro sign has been decoded properly
        CPPUNIT_ASSERT_EQUAL(static_cast<t::utf32_char_t>(0x20ac), target[7]);
    }


    void testDecodeUTF_8()
    {
        namespace t = fhtagn::text;

        // \xe2\x82\xac is the euro sign in UTF-8
        std::string source = "Hello, \xe2\x82\xac world!";
        t::utf32_string target;
        std::string::iterator error_iter = t::decode<t::utf8_decoder>(
                source.begin(), source.end(),
                std::back_insert_iterator<t::utf32_string>(target));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), target.size());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::string::difference_type>(17),
                (error_iter - source.begin()));
        // ensure that the euro sign has been decoded properly
        CPPUNIT_ASSERT_EQUAL(static_cast<t::utf32_char_t>(0x20ac), target[7]);
    }

    void testDecodeUTF_16()
    {
        namespace t = fhtagn::text;

        // unicode code point 119070 (hex 1D11E) is musical G clef, which
        // is an example of code points encoded in four bytes in UTF-16:
        // Numeric values: D834 DD1E
        //
        // LE: 34 D8 1E DD
        char le_source[] = { '\xff', '\xfe', 'H',    '\x00', 'e', '\x00', 'l', '\x00',
                             'l',    '\x00', 'o',    '\x00', ',', '\x00', ' ', '\x00',
                             '\x34', '\xd8', '\x1e', '\xdd', ' ', '\x00', 'w', '\x00',
                             'o',    '\x00', 'r',    '\x00', 'l', '\x00', 'd', '\x00',
                             '!', '\x00' };
        // BE: D8 34 DD 1E
        char be_source[] = { '\xfe', '\xff', '\x00', 'H',    '\x00', 'e', '\x00', 'l',
                             '\x00', 'l',    '\x00', 'o',    '\x00', ',', '\x00', ' ',
                             '\xd8', '\x34', '\xdd', '\x1e', '\x00', ' ', '\x00', 'w',
                             '\x00', 'o',    '\x00', 'r',    '\x00', 'l', '\x00', 'd',
                             '\x00', '!' };

        // UTF-16LE decoding, with a specialized utf16le_decoder
        t::utf32_string target1;
        {
            // add 2 to start & end to skip BOM
            char * error_ptr = t::decode<t::utf16le_decoder>(
                    le_source + 2, le_source + 34,
                    std::back_insert_iterator<t::utf32_string>(target1));
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), target1.size());
            CPPUNIT_ASSERT_EQUAL(error_ptr, le_source + 34);
            // ensure that the G clef sign has been decoded properly
            CPPUNIT_ASSERT_EQUAL(static_cast<t::utf32_char_t>(0x1d11e), target1[7]);
        }

        // UTF-16BE decoding, with a specialized utf16be_decoder
        t::utf32_string target2;
        {
            // add 2 to start & end to skip BOM
            char * error_ptr = t::decode<t::utf16be_decoder>(
                    be_source + 2, be_source + 34,
                    std::back_insert_iterator<t::utf32_string>(target2));
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), target2.size());
            CPPUNIT_ASSERT_EQUAL(error_ptr, be_source + 34);
            // ensure that the G clef sign has been decoded properly
            CPPUNIT_ASSERT_EQUAL(static_cast<t::utf32_char_t>(0x1d11e), target2[7]);
        }

        // compare both conversions
        CPPUNIT_ASSERT(target1 == target2);

        // UTF-16LE decoding based on a BOM
        {
            t::utf32_string target;
            char * error_ptr = t::decode<t::utf16_decoder>(
                    le_source, le_source + 34,
                    std::back_insert_iterator<t::utf32_string>(target));
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), target.size());
            CPPUNIT_ASSERT_EQUAL(error_ptr, le_source + 34);
            // ensure that the G clef sign has been decoded properly
            CPPUNIT_ASSERT_EQUAL(static_cast<t::utf32_char_t>(0x1d11e), target[7]);
        }

        // UTF-16BE decoding based on a BOM
        {
            t::utf32_string target;
            char * error_ptr = t::decode<t::utf16_decoder>(
                    be_source, be_source + 34,
                    std::back_insert_iterator<t::utf32_string>(target));
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), target.size());
            CPPUNIT_ASSERT_EQUAL(error_ptr, be_source + 34);
            // ensure that the G clef sign has been decoded properly
            CPPUNIT_ASSERT_EQUAL(static_cast<t::utf32_char_t>(0x1d11e), target[7]);
        }

    }


    void testDecodeUTF_32()
    {
        namespace t = fhtagn::text;

        // unicode code point 119070 (hex 1D11E) is musical G clef, which
        // is an example of code points encoded in four bytes in UTF-32:
        char le_source[] = "\xff\xfe\0\0H\0\0\0e\0\0\0l\0\0\0l\0\0\0o\0\0\0"
                           ",\0\0\0 \0\0\0\x1e\xd1\x01\0 \0\0\0w\0\0\0o\0\0\0"
                           "r\0\0\0l\0\0\0d\0\0\0!\0\0\0";
        char be_source[] = "\0\0\xfe\xff\0\0\0H\0\0\0e\0\0\0l\0\0\0l\0\0\0o"
                           "\0\0\0,\0\0\0 \0\x01\xd1\x1e\0\0\0 \0\0\0w\0\0\0o"
                           "\0\0\0r\0\0\0l\0\0\0d\0\0\0!";

        // UTF-32LE decoding, with a specialized utf32le_decoder
        t::utf32_string target1;
        {
            // add 2 to start & end to skip BOM
            char * error_ptr = t::decode<t::utf32le_decoder>(
                    le_source + 4, le_source + 64,
                    std::back_insert_iterator<t::utf32_string>(target1));
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), target1.size());
            CPPUNIT_ASSERT_EQUAL(error_ptr, le_source + 64);
            // ensure that the G clef sign has been decoded properly
            CPPUNIT_ASSERT_EQUAL(static_cast<t::utf32_char_t>(0x1d11e), target1[7]);
        }

        // UTF-32BE decoding, with a specialized utf32be_decoder
        t::utf32_string target2;
        {
            // add 2 to start & end to skip BOM
            char * error_ptr = t::decode<t::utf32be_decoder>(
                    be_source + 4, be_source + 64,
                    std::back_insert_iterator<t::utf32_string>(target2));
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), target2.size());
            CPPUNIT_ASSERT_EQUAL(error_ptr, be_source + 64);
            // ensure that the G clef sign has been decoded properly
            CPPUNIT_ASSERT_EQUAL(static_cast<t::utf32_char_t>(0x1d11e), target2[7]);
        }

        // compare both conversions
        CPPUNIT_ASSERT(target1 == target2);

        // UTF-32LE decoding based on a BOM
        {
            t::utf32_string target;
            char * error_ptr = t::decode<t::utf32_decoder>(
                    le_source, le_source + 64,
                    std::back_insert_iterator<t::utf32_string>(target));
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), target.size());
            CPPUNIT_ASSERT_EQUAL(error_ptr, le_source + 64);
            // ensure that the G clef sign has been decoded properly
            CPPUNIT_ASSERT_EQUAL(static_cast<t::utf32_char_t>(0x1d11e), target[7]);
        }

        // UTF-32BE decoding based on a BOM
        {
            t::utf32_string target;
            char * error_ptr = t::decode<t::utf32_decoder>(
                    be_source, be_source + 64,
                    std::back_insert_iterator<t::utf32_string>(target));
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), target.size());
            CPPUNIT_ASSERT_EQUAL(error_ptr, be_source + 64);
            // ensure that the G clef sign has been decoded properly
            CPPUNIT_ASSERT_EQUAL(static_cast<t::utf32_char_t>(0x1d11e), target[7]);
        }
    }


    void testEncodeASCII()
    {
        namespace t = fhtagn::text;

        // simple test, expected to succeed.
        {
            t::utf32_char_t source_array[] = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!', '\0' };
            t::utf32_string source = source_array;

            std::string expected = "Hello, world!";

            std::string target;
            t::utf32_string::const_iterator error_iter = t::encode<t::ascii_encoder>(source.begin(),
                    source.end(), std::back_insert_iterator<std::string>(target));
            CPPUNIT_ASSERT(source.end() == error_iter);
            CPPUNIT_ASSERT_EQUAL(expected, target);
        }

        // test with non-ASCII character (a umlaut). Because the replacement
        // char cannot be encoded in ASCII, we merely expect this char to be
        // skipped in the output
        {
            t::utf32_char_t source_array[] = { 'H', 'e', 'l', 'l', 'o', ',', 0xe4, ' ', 'w', 'o', 'r', 'l', 'd', '!', '\0' };
            t::utf32_string source = source_array;

            std::string expected = "Hello, world!";

            std::string target;
            t::utf32_string::const_iterator error_iter = t::encode<t::ascii_encoder>(source.begin(),
                    source.end(), std::back_insert_iterator<std::string>(target));
            CPPUNIT_ASSERT(source.end() == error_iter);
            CPPUNIT_ASSERT_EQUAL(expected, target);
        }

        // Sometimes you see ASCII text encoding unknown characters as ? - let's
        // see if that works.
        {
            t::utf32_char_t source_array[] = { 'H', 'e', 'l', 'l', 'o', ',', 0xe4, ' ', 'w', 'o', 'r', 'l', 'd', '!', '\0' };
            t::utf32_string source = source_array;

            std::string expected = "Hello,? world!";

            std::string target;
            t::utf32_string::const_iterator error_iter = t::encode<t::ascii_encoder>(source.begin(),
                    source.end(), std::back_insert_iterator<std::string>(target), true, '?');
            CPPUNIT_ASSERT(source.end() == error_iter);
            CPPUNIT_ASSERT_EQUAL(expected, target);
        }

        // lastly, ensure that when we want to fail at invalid characters we
        // fail at the correct input position.
        {
            t::utf32_char_t source_array[] = { 'H', 'e', 'l', 'l', 'o', ',', 0xe4, ' ', 'w', 'o', 'r', 'l', 'd', '!', '\0' };
            t::utf32_string source = source_array;

            std::string expected = "Hello,";

            std::string target;
            t::utf32_string::const_iterator error_iter = t::encode<t::ascii_encoder>(source.begin(),
                    source.end(), std::back_insert_iterator<std::string>(target), false);
            CPPUNIT_ASSERT(source.begin() + 6 == error_iter);
            CPPUNIT_ASSERT_EQUAL(expected, target);
        }
    }

    void testEncodeISO_8859_15()
    {
        namespace t = fhtagn::text;

        // simple test, expected to succeed.
        {
            t::utf32_char_t source_array[] = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!', '\0' };
            t::utf32_string source = source_array;

            std::string expected = "Hello, world!";

            std::string target;
            t::utf32_string::const_iterator error_iter = t::encode<t::iso8859_15_encoder>(source.begin(),
                    source.end(), std::back_insert_iterator<std::string>(target));
            CPPUNIT_ASSERT(source.end() == error_iter);
            CPPUNIT_ASSERT_EQUAL(expected, target);
        }

        // test with non-ASCII character (a umlaut). This must be valid in iso-8859-15
        {
            t::utf32_char_t source_array[] = { 'H', 'e', 'l', 'l', 'o', ',', 0xe4, ' ', 'w', 'o', 'r', 'l', 'd', '!', '\0' };
            t::utf32_string source = source_array;

            std::string expected = "Hello,\xe4 world!";

            std::string target;
            t::utf32_string::const_iterator error_iter = t::encode<t::iso8859_15_encoder>(source.begin(),
                    source.end(), std::back_insert_iterator<std::string>(target));
            CPPUNIT_ASSERT(source.end() == error_iter);
            CPPUNIT_ASSERT_EQUAL(expected, target);
        }

        // test with non-ISO-8859-15 character (G clef) - that's expected to be ignored ...
        {
            t::utf32_char_t source_array[] = { 'H', 'e', 'l', 'l', 'o', ',', 0x1d11e, ' ', 'w', 'o', 'r', 'l', 'd', '!', '\0' };
            t::utf32_string source = source_array;

            std::string expected = "Hello, world!";

            std::string target;
            t::utf32_string::const_iterator error_iter = t::encode<t::iso8859_15_encoder>(source.begin(),
                    source.end(), std::back_insert_iterator<std::string>(target));
            CPPUNIT_ASSERT(source.end() == error_iter);
            CPPUNIT_ASSERT_EQUAL(expected, target);
        }

        // ... or optionally encoded as a ? ...
        {
            t::utf32_char_t source_array[] = { 'H', 'e', 'l', 'l', 'o', ',', 0x1d11e, ' ', 'w', 'o', 'r', 'l', 'd', '!', '\0' };
            t::utf32_string source = source_array;

            std::string expected = "Hello,? world!";

            std::string target;
            t::utf32_string::const_iterator error_iter = t::encode<t::iso8859_15_encoder>(source.begin(),
                    source.end(), std::back_insert_iterator<std::string>(target), true, '?');
            CPPUNIT_ASSERT(source.end() == error_iter);
            CPPUNIT_ASSERT_EQUAL(expected, target);
        }

        // ... or encoding breaks off at that point
        {
            t::utf32_char_t source_array[] = { 'H', 'e', 'l', 'l', 'o', ',', 0x1d11e, ' ', 'w', 'o', 'r', 'l', 'd', '!', '\0' };
            t::utf32_string source = source_array;

            std::string expected = "Hello,";

            std::string target;
            t::utf32_string::const_iterator error_iter = t::encode<t::iso8859_15_encoder>(source.begin(),
                    source.end(), std::back_insert_iterator<std::string>(target), false);
            CPPUNIT_ASSERT(source.begin() + 6 == error_iter);
            CPPUNIT_ASSERT_EQUAL(expected, target);
        }
    }

};


CPPUNIT_TEST_SUITE_REGISTRATION(TextTest);
