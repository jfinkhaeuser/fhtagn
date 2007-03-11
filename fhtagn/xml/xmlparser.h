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

class xmlparser
{
public:
	xmlparser();
	virtual ~xmlparser();
	
	bool parse(const std::string& data);
};

#endif