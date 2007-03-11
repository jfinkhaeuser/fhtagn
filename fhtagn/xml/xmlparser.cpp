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

xmlparser::xmlparser()
{

}

xmlparser::~xmlparser()
{

}

bool xmlparser::parse(const std::string& data)
{
	xmlgrammar xmlgr;
	BOOST_SPIRIT_DEBUG_NODE(xmlgr);		 
	return boost::spirit::parse(data.c_str(), xmlgr).full;
}
