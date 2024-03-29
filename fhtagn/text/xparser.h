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
#ifndef FHTAGN_TEXT_XPARSER_H
#define FHTAGN_TEXT_XPARSER_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <fhtagn/fhtagn.h>

#include <boost/version.hpp>
#if BOOST_VERSION >= 103800
#define BOOST_SPIRIT_USE_OLD_NAMESPACE
#include <boost/spirit/include/classic_core.hpp>
#else
#include <boost/spirit/core.hpp>
#endif

#include <fhtagn/text/decoders.h>


namespace fhtagn {
namespace text {


/**
 * Simple predefined boost::spirit parser matching byte order marks (BOMs) for
 * various character encodings.
 *
 * If the bom_parser matches any known BOM sequence, it's encoding member is set
 * to reflect the associated encoding - e.g. UTF-32LE, etc.
 *
 * If the bom_parser does not match any known BOM sequence, matching fails, and
 * the encoding member is set to the RAW char_encoding_type.
 **/
struct bom_parser
    : public boost::spirit::grammar<bom_parser>
{
    bom_parser()
        : encoding(RAW)
    {
    }

    inline void set_encoding(char_encoding_type enc) const
    {
        encoding = enc;
    }

    template <typename ScannerT>
    struct definition
    {
        definition(bom_parser const & self)
        {
            utf32_be_rule = boost::spirit::ch_p(utf32_be_bom[0])
                    >> boost::spirit::ch_p(utf32_be_bom[1])
                    >> boost::spirit::ch_p(utf32_be_bom[2])
                    >> boost::spirit::ch_p(utf32_be_bom[3]);
            utf32_le_rule = boost::spirit::ch_p(utf32_le_bom[0])
                    >> boost::spirit::ch_p(utf32_le_bom[1])
                    >> boost::spirit::ch_p(utf32_le_bom[2])
                    >> boost::spirit::ch_p(utf32_le_bom[3]);
            utf16_be_rule = boost::spirit::ch_p(utf16_be_bom[0])
                    >> boost::spirit::ch_p(utf16_be_bom[1]);
            utf16_le_rule = boost::spirit::ch_p(utf16_le_bom[0])
                    >> boost::spirit::ch_p(utf16_le_bom[1]);
            utf8_rule = boost::spirit::ch_p(utf8_bom[0])
                    >> boost::spirit::ch_p(utf8_bom[1])
                    >> boost::spirit::ch_p(utf8_bom[2]);

            main_rule = utf32_be_rule[boost::bind(&bom_parser::set_encoding,
                        boost::ref(self), UTF_32BE)]
                    | utf32_le_rule[boost::bind(&bom_parser::set_encoding,
                        boost::ref(self), UTF_32LE)]
                    | utf16_be_rule[boost::bind(&bom_parser::set_encoding,
                        boost::ref(self), UTF_16BE)]
                    | utf16_le_rule[boost::bind(&bom_parser::set_encoding,
                        boost::ref(self), UTF_16LE)]
                    | utf8_rule[boost::bind(&bom_parser::set_encoding,
                        boost::ref(self), UTF_8)];

            BOOST_SPIRIT_DEBUG_NODE(utf32_be_rule);
            BOOST_SPIRIT_DEBUG_NODE(utf32_le_rule);
            BOOST_SPIRIT_DEBUG_NODE(utf16_be_rule);
            BOOST_SPIRIT_DEBUG_NODE(utf16_le_rule);
            BOOST_SPIRIT_DEBUG_NODE(utf8_rule);
            BOOST_SPIRIT_DEBUG_NODE(main_rule);
        }

        boost::spirit::rule<ScannerT> utf32_be_rule;
        boost::spirit::rule<ScannerT> utf32_le_rule;
        boost::spirit::rule<ScannerT> utf16_be_rule;
        boost::spirit::rule<ScannerT> utf16_le_rule;
        boost::spirit::rule<ScannerT> utf8_rule;
        boost::spirit::rule<ScannerT> main_rule;

        boost::spirit::rule<ScannerT> const & start() const { return main_rule; }
    };

    mutable char_encoding_type encoding;
};



/**
 * Forward declaration, see documentation below
 **/
class char_parser_factory;



/**
 * The char_parser is a specialization of the char_parser class template from
 * boost::spirit. It's specialties are that
 *   a) it can only be applied to char sequences (not wchar_t or any other
 *      character type), and
 *   b) that it outputs utf32_char_t.
 *
 * Depending on a runtime switch, it will interpret char sequences as utf8,
 * utf16, or a number of other character encodings, and transcode these to
 * utf32_sequences.
 *
 * Due to the way parser grammars are commonly used with boost::spirit, the
 * char_parser class cannot be instanciated directly. In most grammars, you
 * will find definitions such as, e.g.
 *
 *    my_rule = ch_p('x') | 'y' | 'z';
 *
 * What's not immediately apparent is that you want "myrule" to match an 'x',
 * 'y' or 'z' character, where each of those possible matches are encoded
 * in the same way.
 *
 * To facilitate such a use, char_parser below does not contain the runtime
 * flag which input encoding to expect in it's own class body, but rather
 * references a char_parser_factory instance that's passed to it in it's
 * constructor. The char_parser_factory class contains factory methods for
 * different types of char_parser instances, such that your rule definition
 * becomes
 *
 *    my_rule = my_factory_instance.ch_p('x') | 'y' | 'z'
 *
 * Automatically, 'x', 'y' and 'z' are attempted to be matched in the same
 * character encoding, and the choice of encoding to match is defined via
 * a member of my_factory_instance.
 *
 * One note of warning: for this to work, my_factory_instance must have an
 * equal or longer lifetime than my_rule - if you create a full-blown grammar,
 * it's recommended that the factory instance becomes a member of said grammar
 * class.
 **/
template <
    typename derived_parserT
>
struct char_parser
    : public boost::spirit::parser<derived_parserT>
{
    typedef derived_parserT self_t;

    template <typename ScannerT>
    struct result
    {
        /**
         * Using utf32_char_t here ensures that the return type of parse() below
         * expects a match for utf32_char_t characters.
         **/
        typedef typename boost::spirit::match_result<
            ScannerT, utf32_char_t
        >::type type;
    };

    template <typename ScannerT>
    typename boost::spirit::parser_result<self_t, ScannerT>::type
    parse(ScannerT const & scan) const;

    char_parser(char_parser_factory * factory)
        : m_factory(factory)
    {
    }
private:
    friend class char_parser_factory;


    char_parser_factory * m_factory;
};




/**
 * To use the char_parser template above, simply place a char_parser_factory
 * instance into your grammar class. Then, instead of using boost::spirit's
 * anychar_p or ch_p, use the factory functions below.
 *
 * All parsers created by one factory share the same decoder, and thus the
 * same expected encoding.
 *
 * If you wish to create a grammar that contains several encodings, but don't
 * want to switch the encoding back and forth at runtime, you need to create
 * several char_parser_factories.
 **/
class char_parser_factory
{
public:
    char_parser_factory(char_encoding_type input_encoding = ISO_8859_1)
        : anychar_p(this)
        , decoder(input_encoding)
    {
    }

    /**
     * Delegate to the decoder instance.
     **/
    char_encoding_type get_encoding() const
    {
        return decoder.get_encoding();
    }

    /**
     * Delegate to the decoder instance. Can be used to switch the expected
     * encoding at any time.
     **/
    void set_encoding(char_encoding_type new_encoding)
    {
        decoder.set_encoding(new_encoding);
    }


    /**
     * Semantically similar to boost::spirit::anychar_parser, except it produces
     * utf32_char_t output from char sequences in different character encodings.
     **/
    struct anychar_parser
        : public char_parser<anychar_parser>
    {
        typedef anychar_parser self_t;

        bool test(utf32_char_t) const
        {
            return true;
        }

    private:
        friend class char_parser_factory;

        anychar_parser(char_parser_factory * factory)
            : char_parser<anychar_parser>(factory)
        {
        }
    };


    /**
     * An instance of anychar_parser; much as in the boost::spirit example,
     * you'll want to use anychar_p in your grammar definitions.
     **/
    anychar_parser anychar_p;


    /**
     * Semantically similar to boost::spirit::chlit, except it matches the
     * utf32_char_t passed to it's constructor. See ch_p() below.
     **/
    struct chlit
        : public char_parser<chlit>
    {
        typedef anychar_parser self_t;

        bool test(utf32_char_t ch) const
        {
            return (ch == m_ch);
        }

        utf32_char_t m_ch;

    private:
        friend class char_parser_factory;

        chlit(char_parser_factory * factory, utf32_char_t ch)
            : char_parser<chlit>(factory)
            , m_ch(ch)
        {
        }
    };


    /**
     * Much like it's boost::spirit counterpart, chlit instances are seldom
     * created directly, but most commonly via a ch_p() factory function. This
     * version of ch_p() has identical semantics, but returns chlit instances
     * that match the passed character in any character encoding.
     **/
    chlit ch_p(utf32_char_t ch)
    {
        return chlit(this, ch);
    }

    /**
     * The universal_decoder instance's encoding determines how parsers created
     * by this instance parse character data.
     **/
    universal_decoder decoder;
};



/**
 * Implementation of char_parser<>::parse - now that the factory class above is
 * defined, this will not choke compilers.
 **/
template <typename derived_parserT>
template <typename ScannerT>
typename boost::spirit::parser_result<
    typename char_parser<derived_parserT>::self_t,
    ScannerT
>::type
char_parser<derived_parserT>::parse(ScannerT const & scan) const
{
    typedef typename boost::spirit::parser_result<self_t, ScannerT>::type result_t;
    // XXX value_t == char
    typedef typename ScannerT::value_t value_t;
    typedef typename ScannerT::iterator_t iterator_t;

    m_factory->decoder.reset();
    iterator_t save(scan.first);

    while (!scan.at_end() && m_factory->decoder.append(*scan)) {
        ++scan.first;
    }

    if (m_factory->decoder.have_full_sequence()) {
        utf32_char_t utf32_ch = m_factory->decoder.to_utf32();
        if (!this->derived().test(utf32_ch)) {
            return scan.no_match();
        }
        return scan.create_match(1, utf32_ch, save, scan.first);
    }
    return scan.no_match();
}



}} // namespace fhtagn::text

#endif // guard
