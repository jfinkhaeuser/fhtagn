#ifndef FHTAGN_XML_DEFAULTHANDLER_H
#define FHTAGN_XML_DEFAULTHANDLER_H

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