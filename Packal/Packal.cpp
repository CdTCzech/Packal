#include "Logger.h"
#include "Parser.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

using namespace std::string_literals;


int main(const int argc, const char** argv)
{
	if (argc != 2)
	{
		LOG(Logger::Type::Error, "Usage: Pascal.exe inputFileName");
		return 1;
	}

	if (!std::filesystem::exists(argv[1]) || !std::filesystem::is_regular_file(argv[1]))
	{
		LOG(Logger::Type::Error, "File does not exist: "s + argv[1]);
		return 1;
	}

	std::ifstream inputFile(argv[1], std::ios::in | std::ios::binary);
	if (!inputFile.is_open() || !inputFile.good())
	{
		LOG(Logger::Type::Error, "Could not open file: "s + argv[1]);
		return 1;
	}

	Parser parser(std::move(inputFile));
	parser.parse();
	
	return 0;
}