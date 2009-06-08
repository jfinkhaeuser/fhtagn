/**
 * $Id$
 *
 * This file is part of the Fhtagn! C++ Library.
 * Copyright (C) 2009 Jens Finkhaeuser <unwesen@users.sourceforge.net>.
 *
 * Author: Tony Kostanjsek <lobotony@users.sourceforge.net>
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
#ifndef FHTAGN_XML_GRAMMAR_H
#define FHTAGN_XML_GRAMMAR_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <fhtagn/fhtagn.h>

#include <string>
#include <map>
#include <iostream>

#include <boost/spirit/core.hpp>
#include <boost/spirit/utility/lists.hpp>
#include <boost/spirit/attribute.hpp>
#include "boost/spirit/phoenix/binders.hpp"

using boost::spirit::grammar;
using boost::spirit::rule;
using boost::spirit::str_p;
using boost::spirit::eps_p;
using boost::spirit::ch_p;
using boost::spirit::ch_p;
using boost::spirit::anychar_p;
using boost::spirit::alnum_p;
using boost::spirit::space_p;
using boost::spirit::f_str_p;
using boost::spirit::closure;

using phoenix::arg1;
using phoenix::arg2;
using phoenix::construct_;
using phoenix::bind;
using phoenix::var;

////////////////////
/// from http://article.gmane.org/gmane.comp.parsers.spirit.general/6051/match=map
namespace phoenix {

///////////////////////////////////////////////////////////////////////////////
//
//  specialization for std::map::operator[]
//
///////////////////////////////////////////////////////////////////////////////
template <typename T0, typename T1>
struct binary_operator<index_op, T0, T1> {

    typedef typename T0::mapped_type& result_type;
    static result_type eval(T0& container, T1 const& key)
    { return container[key]; }
};

}
//////////////////////////////////

namespace fhtagn
{

namespace xml
{

struct node_closure : closure<node_closure, std::string, std::map<std::string, std::string> >
{
	member1 tag;
	member2 attributes;
};

struct attribute_closure : closure<attribute_closure, std::string, std::string >
{
	member1 key;
	member2 val;
};

template<class HANDLER>
struct grammar : public boost::spirit::grammar<xml::grammar<HANDLER> >
{
	HANDLER& handler;
	grammar(HANDLER& h) : handler(h) {};

	template <typename ScannerT>
	struct definition
	{
		definition(xml::grammar<HANDLER> const& self) 
		{ 
			processing_instruction = str_p("<?") >> +(anychar_p-"?>") >> "?>";
			doctype = str_p("<!") >> +(anychar_p-'>') >> '>';
			quoted_string = ch_p("\"") >> *(anychar_p-"\"") >> "\"";
			attribute_keyword = +(alnum_p|'_'|':');
			attribute = (   attribute_keyword[attribute.key=construct_<std::string>(arg1, arg2)]
						>> *space_p
						>> '='
						>> *space_p
						>> quoted_string[attribute.val=construct_<std::string>(arg1, arg2)])
						[node.attributes[attribute.key] = attribute.val];
			data = *((alnum_p|'.')-'<');
			tagname = +(alnum_p|'_');
			open_tag =		ch_p('<') 
						>>  tagname[node.tag = construct_<std::string>(arg1, arg2)] 
						>> !(+space_p >> list_p(attribute, +space_p)) 
						>> *space_p
						>> '>';
			close_tag = ch_p('<') >> '/' >> f_str_p(node.tag) >> *space_p >> '>';
			empty_node =(   ch_p('<') 
						>> tagname[node.tag = construct_<std::string>(arg1, arg2)]
						>> !(+space_p >> list_p(attribute, +space_p))
						>> *space_p
						>> '/'
						>> '>')
						[bind(&HANDLER::start_element)(var(self.handler), node.tag, node.attributes)]
						[bind(&HANDLER::end_element)(var(self.handler), node.tag)];
			node_content = list_p(node, *space_p) 
						   | 
						   data[bind(&HANDLER::characters)(var(self.handler), construct_<std::string>(arg1, arg2))];
			full_node =	   open_tag[bind(&HANDLER::start_element)(var(self.handler), node.tag, node.attributes)]
						>> *space_p 
						>> node_content 
						>> *space_p 
						>> close_tag[bind(&HANDLER::end_element)(var(self.handler), node.tag)];
			node = empty_node | full_node;
			xmldocument =      eps_p	[bind(&HANDLER::start_document)(var(self.handler))]
							>> *space_p
							>> !list_p(processing_instruction, *space_p)
							>> *space_p 
							>> !doctype 
							>> *space_p
							>> +list_p(node, *space_p)
							>> *space_p
							>> eps_p   [bind(&HANDLER::end_document)(var(self.handler))]; 
								 
			BOOST_SPIRIT_DEBUG_NODE(processing_instruction);
			BOOST_SPIRIT_DEBUG_NODE(doctype);
			BOOST_SPIRIT_DEBUG_NODE(quoted_string);
			BOOST_SPIRIT_DEBUG_NODE(attribute_keyword);
			BOOST_SPIRIT_DEBUG_NODE(attribute);
			BOOST_SPIRIT_DEBUG_NODE(data);
			BOOST_SPIRIT_DEBUG_NODE(tagname);
			BOOST_SPIRIT_DEBUG_NODE(open_tag);
			BOOST_SPIRIT_DEBUG_NODE(close_tag);
			BOOST_SPIRIT_DEBUG_NODE(empty_node);
			BOOST_SPIRIT_DEBUG_NODE(node);
			BOOST_SPIRIT_DEBUG_NODE(node_content);
			BOOST_SPIRIT_DEBUG_NODE(full_node);
			BOOST_SPIRIT_DEBUG_NODE(xmldocument);		 
		}

		rule<ScannerT> processing_instruction;
		rule<ScannerT> doctype;
		rule<ScannerT> quoted_string;
		rule<ScannerT> attribute_keyword;
		rule<ScannerT, attribute_closure::context_t> attribute;
		rule<ScannerT> data;
		rule<ScannerT> tagname;
		rule<ScannerT> open_tag;
		rule<ScannerT> close_tag;
		rule<ScannerT> empty_node;
		rule<ScannerT, node_closure::context_t> node;
		rule<ScannerT> node_content;
		rule<ScannerT> full_node;
		rule<ScannerT> xmldocument;
		
		rule<ScannerT> const& start() const { return xmldocument; }
	};
};

} // xml

} // fhtagn

#endif 
