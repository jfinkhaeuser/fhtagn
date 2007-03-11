/*
 *  xmlparser.h
 *  xmlparser
 *
 *  Created by Tony Kostanjsek on 10.03.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef XMLPARSER_H
#define XMLPARSER_H

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