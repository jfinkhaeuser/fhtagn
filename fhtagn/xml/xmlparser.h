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

namespace fhtagn
{

namespace xml
{

class parser
{
public:
	parser();
	virtual ~parser();
	
	bool parse(const std::string& data);
};

} // xml

} // fhtagn

#endif