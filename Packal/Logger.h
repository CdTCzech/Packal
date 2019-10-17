#pragma once

#include <iostream>

#define LOG(type, message) Logger::instance().log(type, message, __FILE__, __LINE__);


class Logger
{
public:
	enum class Type
	{
		Info,
		Warning,
		Error
	};

public:
	static Logger& instance()
	{
		static Logger logger;
		return logger;
	}

	void log(Type type, const std::string& message, const char* file, uint64_t line)
	{
		std::string typeString;
		switch(type)
		{
		case Type::Info:
			typeString = "INFO(";
			break;
		case Type::Warning:
			typeString = "WARNING(";
			break;
		case Type::Error:
			typeString = "ERROR(";
			break;
		default:
			typeString = "UNKNOWN(";
		}

		std::cout << typeString << file << "," << line << "): " << message << std::endl;
	}

private:
	Logger() = default;
};