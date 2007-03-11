/*
 *  xmlparser.cpp
 *  xmlparser
 *
 *  Created by Tony Kostanjsek on 10.03.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

//#define BOOST_SPIRIT_DEBUG

#include "xmlparser.h"
#include "xmlgrammar.h"

namespace fhtagn
{

namespace xml
{


parser::parser()
{

}

parser::~parser()
{

}

bool parser::parse(const std::string& data)
{
	fhtagn::xml::grammar xmlgr;
	BOOST_SPIRIT_DEBUG_NODE(xmlgr);		 
	return boost::spirit::parse(data.c_str(), xmlgr).full;
}

} // xml

} // fhtagn
