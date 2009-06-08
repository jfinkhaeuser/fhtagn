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
#ifndef FHTAGN_XML_PARSER_H
#define FHTAGN_XML_PARSER_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <fhtagn/fhtagn.h>

#include <string>
#include "fhtagn/xml/parser.h"
#include "fhtagn/xml/grammar.h"

namespace fhtagn
{

namespace xml
{

template<class HANDLER>
struct parser
{	
	bool parse(const std::string& data, HANDLER& handler)
	{
		fhtagn::xml::grammar<HANDLER> xmlgr(handler);
		BOOST_SPIRIT_DEBUG_NODE(xmlgr);		 
		return boost::spirit::parse(data.c_str(), xmlgr).full;	
	}
};

} // xml

} // fhtagn

#endif
