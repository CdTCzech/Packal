#pragma once

#include "Logger.h"

#include <fstream>
#include <string>
#include <vector>

using namespace std::string_literals;


class Lexer
{
public:
	enum class Type
	{
		Unknown,
		Identifier,
		Number,
		String,
		Operator,
		Program,
		Function,
		Procedure,
		Var,
		Const,
		Begin,
		End,
		Integer,
		Readln,
		Writeln,
		Eof
	};

	struct Token
	{
		Type type = Type::Unknown;
		std::string value;
	};

public:
	static std::string toString(Type type)
	{
		switch (type)
		{
		case Type::Unknown:
			return "Unknown";
		case Type::Identifier:
			return "Identifier";
		case Type::Number:
			return "Number";
		case Type::String:
			return "String";
		case Type::Operator:
			return "Operator";
		case Type::Program:
			return "Program";
		case Type::Function:
			return "Function";
		case Type::Procedure:
			return "Procedure";
		case Type::Var:
			return "Var";
		case Type::Const:
			return "Const";
		case Type::Begin:
			return "Begin";
		case Type::End:
			return "End";
		case Type::Integer:
			return "Integer";
		case Type::Readln:
			return "Readln";
		case Type::Writeln:
			return "Writeln";
		case Type::Eof:
			return "Eof";
		default:
			LOG(Logger::Type::Error, "Unknown token type");
			return "Unknown";
		}
	}

	Lexer(std::ifstream&& input) : m_input(std::move(input)), m_buffer(SIZE65KB)
	{}

	Token nextToken()
	{
		Token token;
		try
		{
			token = next();
		}
		catch (std::exception e)
		{
			token = { Type::Eof, {} };
		}
		return refine(token);
	}

private:
	void fetchBuffer()
	{
		if (m_index == m_size || m_index + SIZE1KB > SIZE65KB)
		{
			const auto left = m_size - m_index;
			memcpy(m_buffer.data(), m_buffer.data() + m_index, left);
			m_input.read(m_buffer.data() + left, SIZE65KB - left);
			m_size = left + static_cast<uint32_t>(m_input.gcount());
			m_index = 0;
		}
		if (m_input.eof() && m_size == 0)
		{
			throw std::exception("EOF");
		}
	}

	Token next()
	{
		fetchBuffer();

		while (std::isspace(m_buffer[m_index]))
		{
			if (++m_index == m_size)
			{
				fetchBuffer();
			}
		}

		if (std::isalpha(m_buffer[m_index]))
		{
			std::string result;
			auto start = m_index++;

			while (std::isalnum(m_buffer[m_index++]))
			{
				if (m_index == m_size)
				{
					result += std::string(m_buffer.data() + start, m_buffer.data() + m_index);
					fetchBuffer();
					start = 0;
				}
			}

			--m_index;
			return { Type::String, result + std::string(m_buffer.data() + start, m_buffer.data() + m_index) };
		}
		else if (std::isdigit(m_buffer[m_index]))
		{
			std::string result;
			auto start = m_index++;

			while (std::isdigit(m_buffer[m_index++]))
			{
				if (m_index == m_size)
				{
					result += std::string(m_buffer.data() + start, m_buffer.data() + m_index);
					fetchBuffer();
					start = 0;
				}
			}

			--m_index;
			return { Type::Number, result + std::string(m_buffer.data() + start, m_buffer.data() + m_index) };
		}
		else if (std::ispunct(m_buffer[m_index]))
		{
			std::string result;
			auto start = m_index++;

			while (std::ispunct(m_buffer[m_index++]))
			{
				if (m_index == m_size)
				{
					result += std::string(m_buffer.data() + start, m_buffer.data() + m_index);
					fetchBuffer();
					start = 0;
				}
			}

			--m_index;
			return { Type::Operator, result + std::string(m_buffer.data() + start, m_buffer.data() + m_index) };
		}
		else
		{
			LOG(Logger::Type::Error, "Unknown character \'"s + m_buffer[m_index] + "\'");
			if (++m_index == m_size)
			{
				fetchBuffer();
			}
			return {};
		}
	}

	Token refine(Token& token)
	{
		switch (token.type)
		{
		case Type::String:
			if (token.value == "program") token.type = Type::Program;
			else if (token.value == "var") token.type = Type::Var;
			else if (token.value == "integer") token.type = Type::Integer;
			else if (token.value == "begin") token.type = Type::Begin;
			else if (token.value == "end") token.type = Type::End;
			else if (token.value == "readln") token.type = Type::Readln;
			else if (token.value == "writeln") token.type = Type::Writeln;
			else token.type = Type::Identifier;
		case Type::Number:
			return token;
		case Type::Operator:
			return token;
		case Type::Eof:
			return token;
		default:
			auto type = toString(token.type);
			LOG(Logger::Type::Error, "Unknown token to refine (type: "s + type + ", value: " + token.value + ")");
		}
		return token;
	}

private:
	std::ifstream m_input;
	std::vector<char> m_buffer;
	size_t m_index{ 0 };
	size_t m_size{ 0 };
	bool m_finished{ false };

	static constexpr uint64_t SIZE1KB = 1 << 10;
	static constexpr uint64_t SIZE64KB = 1 << 16;
	static constexpr uint64_t SIZE65KB = SIZE64KB + SIZE1KB;
};