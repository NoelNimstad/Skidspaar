#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "fs.hpp"
#include "debug.hpp"

namespace Spår::Utility
{
std::string readFile(const std::string &path)
{
	std::ifstream stream(path, std::ios::binary);
	if(!stream)
	{
		Debug::printError("Utility::readFile", "Failed to open file: " + path);
		return {};
	}

	stream.seekg(0, std::ios::end);
	std::streampos fileSize = stream.tellg();
	stream.seekg(0, std::ios::beg);

	std::string content;
	if(fileSize > 0)
	{
		content.resize(static_cast<size_t>(fileSize));
		stream.read(&content[0], static_cast<std::streamsize>(fileSize));
	}
	
	return content;
}

void readFileStream(const std::string &path, std::ostream &output)
{
	std::ifstream stream(path, std::ios::binary);
	if(!stream)
	{
		Debug::printError("Utility::readFileStream", "Failed to open file: " + path);
		return;
	}

	constexpr size_t bufferSize = 2 << 11;
	char buffer[bufferSize];
	while(stream.read(buffer, bufferSize))
	{
		output.write(buffer, static_cast<std::streamsize>(stream.gcount()));
	}

	if(stream.gcount() > 0)
	{
		output.write(buffer, static_cast<std::streamsize>(stream.gcount()));
	}
}
}