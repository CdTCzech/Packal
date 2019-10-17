#include "Lexer.h"
#include "Logger.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>


int main(const int argc, const char** argv)
{
	if (argc != 2)
	{
		std::cout << "Usage: Pascal.exe inputFileName" << std::endl;
		return 1;
	}

	if (!std::filesystem::exists(argv[1]) || !std::filesystem::is_regular_file(argv[1]))
	{
		std::cout << "File does not exist: " << argv[1] << std::endl;
		return 1;
	}

	std::ifstream inputFile(argv[1], std::ios::in | std::ios::binary);
	if (!inputFile.is_open() || !inputFile.good())
	{
		std::cout << "Could not open file: " << argv[1] << std::endl;
		return 1;
	}

	Lexer lexer(std::move(inputFile));
	auto token = lexer.nextToken();

	while (token.type != Lexer::Type::Eof)
	{
		auto typeString = Lexer::toString(token.type);
		LOG(Logger::Type::Info, "Type: " + typeString + ", Value: " + token.value);
		token = lexer.nextToken();
	}

	return 0;
}