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
#ifndef FHTAGN_XML_DEFAULTHANDLER_H
#define FHTAGN_XML_DEFAULTHANDLER_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <fhtagn/fhtagn.h>

#include <string>
#include <iostream>
#include <map>
#include <vector>

namespace fhtagn
{

namespace xml
{


struct defaulthandler {

	mutable std::map<std::string, long> tagcount;	
		
	void start_document() const
	{
		std::cout << "starting parser" << std::endl;
	}

	void end_document() const
	{
		std::cout << "tag counts: " << std::endl;
		std::map<std::string, long>::const_iterator i;
		for(i = tagcount.begin(); i != tagcount.end(); ++i)
		{
			std::cout << i->first << " : " << i->second << std::endl;
		}
	}

	void start_element(const std::string& name, const std::map<std::string, std::string>& attributes) const
	{
		std::cout << "start element: '" << name << "' ";
		std::map<std::string, std::string>::const_iterator i;
		for(i = attributes.begin(); i != attributes.end(); ++i)
		{
			std::cout <<"(" << i->first << " = " << i->second <<")";
		}
		std::cout << std::endl;
		tagcount[name] += 1;
	}

	void end_element(const std::string& name) const
	{
		std::cout << "end element: '" << name << "'" << std::endl;
	}
	
	void characters(const std::string& name) const
	{
		std::cout << "characters: '" << name << "'" << std::endl;
	}		
};

} // xml

} // fhtagn

#endif
