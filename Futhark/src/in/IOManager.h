#pragma once
#include <vector>
#include <string>
#include "../base/Utility.h"
namespace fk {


/* Used to load input files.
[t3chma] */
class IOManager {
	
public:
		
	/* Reads a binary file into a vector of chars.
	(filePath) The path to the file to load.
	(buffer)< The vector to fill.
	< If the file read was successful
	[t3chma] */
	bool readBinaryFileToBuffer(
		const std::string& filePath,
		std::vector<unsigned char>& buffer
	);
		
	/* Reads a binary file into a string.
	(filePath) The path to the file to load.
	(fileContents)< The string to fill.
	< If the file read was successful
	[t3chma] */
	bool readTextFileToString(
		const std::string& filePath, 
		std::string& fileContents
	);
		
	/* Overwrite a string to a file.
	(filePath) The path to the file to overwrite.
	(string) The string to write.
	< If the file write was successful
	[t3chma] */
	bool overwriteStringToFile(
		const std::string& FILE_PATH,
		std::string& string
	);
		
	/* Reads a JSON, XML, or INI file into a Dictionary object.
	^ http://www.boost.org/doc/libs/1_55_0/boost/property_tree/ptree.hpp
	^ http://www.boost.org/doc/libs/1_42_0/doc/html/boost_propertytree/tutorial.html
	^ http://www.boost.org/doc/libs/1_55_0/boost/property_tree/json_parser.hpp
	(filePath) The path to the file to load.
	(fileContents)< The string to fill.
	< If the file read was successful
	[t3chma] */
	bool readInDict(
		const std::string& filePath,
		Dict& fileContents
	);

};

}