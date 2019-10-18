#pragma once

#include <iostream>
#include <string>

#define LOG(type, message) Logger::instance().log(type, message, __FILE__, __LINE__);
#define THROW_LOG(type, message) throw Logger::LogException(type, message, __FILE__, __LINE__);


class Logger
{
public:
	enum class Type
	{
		Unknown,
		Info,
		Warning,
		Error
	};

	class LogException
	{
	public:
		LogException(Type type, const std::string& message, const std::string& file, uint64_t line)
			: m_type(type), m_message(message), m_file(file), m_line(line)
		{}

	public:
		Type m_type{ Type::Unknown };
		std::string m_message;
		std::string m_file;
		uint64_t m_line;
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