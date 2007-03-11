#include <iostream>
#include <string>
#include <stdexcept>
#include <fstream>
#include <sstream>

#include "fhtagn/xml/parser.h"

using namespace std;

int main (int argc, char * const argv[])
{
	try
	{
		fhtagn::xml::parser parser;
	
		string filename = argv[1];
		ifstream file(filename.c_str());
		if(!file) throw runtime_error("couldn't open file: '"+filename+"'");
		ostringstream os;
		os << file.rdbuf();
		cout << "----- loaded data -----" << endl;
		cout << os.str() << endl;
		cout << "----- ends ------------" << endl;
		
		if(!parser.parse(os.str())) throw runtime_error("couldn't fully parse document");
		
		cout << " -- Ok -- " << endl;
		
		return 0;
	}
	catch(exception& ex)
	{
		cout << "caught error: " << ex.what() << endl;
		return 1;
	}
}
