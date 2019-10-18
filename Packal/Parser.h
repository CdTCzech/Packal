#pragma once

#include "Lexer.h"
#include "Logger.h"

#include <map>
#include <set>
#include <string>
#include <vector>


enum class VariableType
{
	Unknown,
	Array,
	Integer
};

struct GlobalVariable
{};

struct GlobalConstant : public GlobalVariable
{
	GlobalConstant(int64_t value) : value(value)
	{}

	int64_t value;
};

struct GlobalInteger : public GlobalVariable
{};

struct GlobalArray : public GlobalVariable
{
	GlobalArray(int64_t start, int64_t end) : start(start), end(end)
	{}

	int64_t start;
	int64_t end;
};

enum class GlobalDeclarationTypes
{
	Unknown,
	Array,
	Constant,
	Integer
};

struct GlobalDeclarations
{
	std::map<std::string, GlobalDeclarationTypes> names;
	std::map<std::string, GlobalVariable> variables;
};

struct Block
{
};

struct Program
{
	std::string name;
	GlobalDeclarations* globalDeclarations{ nullptr };
	Block* block{ nullptr };
};

class Parser
{
public:
	Parser(std::ifstream&& input) : m_lexer(Lexer(std::move(input)))
	{}

	~Parser()
	{
		delete m_program.globalDeclarations;
		delete m_program.block;
	}

	void parse()
	{
		next();
		try
		{
			parseProgram();
		}
		catch(Logger::LogException& e)
		{
			LOG(e.m_type, e.m_message, e.m_file, e.m_line);
		}
	}

private:
	void next()
	{
		m_token = m_lexer.nextToken();
	}

	GlobalDeclarations* globalDeclarations()
	{
		return m_program.globalDeclarations;
	}

	void parseProgram()
	{
		if (m_token.type != Lexer::Type::Program) THROW_LOG(Logger::Type::Error, R"(Expected 'program' when parsing program)");
		next();

		if (m_token.type != Lexer::Type::Identifier) THROW_LOG(Logger::Type::Error, R"(Expected identifier when parsing program)");
		m_program.name = m_token.value;
		next();

		if (m_token.type != Lexer::Type::Operator || m_token.value != ";") THROW_LOG(Logger::Type::Error, R"(Expected operator ';' when parsing program)");
		next();

		m_program.globalDeclarations = new GlobalDeclarations();
		parseGlobalDeclarations();
		m_program.block = new Block();
		parseBlock();

		if (m_token.type != Lexer::Type::Operator || m_token.value != ".") THROW_LOG(Logger::Type::Error, R"(Expected operator '.' when parsing program)");
	}

	void parseGlobalDeclarations()
	{
		parseGlobalFunction();
		parseGlobalProcedure();
		parseGlobalConstant();
		parseGlobalVariable();
	}

	void parseGlobalFunction()
	{
		if (m_token.type != Lexer::Type::Function) return;
	}

	void parseGlobalProcedure()
	{
		if (m_token.type != Lexer::Type::Procedure) return;
	}

	void parseGlobalConstant()
	{
		if (m_token.type != Lexer::Type::Const) return;
		next();

		std::set<std::string> variables;
		if (m_token.type != Lexer::Type::Identifier) THROW_LOG(Logger::Type::Error, R"(Expected identifier when parsing constant)");
		variables.emplace(m_token.value);
		next();

		while (m_token.type == Lexer::Type::Operator && m_token.value == ",")
		{
			next();
			if (m_token.type != Lexer::Type::Identifier) THROW_LOG(Logger::Type::Error, R"(Expected identifier when parsing constant)");
			if (variables.contains(m_token.value)) return;
			variables.emplace(m_token.value);
			next();
		}

		if (m_token.type != Lexer::Type::Operator || m_token.value != "=") THROW_LOG(Logger::Type::Error, R"(Expected operator '=' when parsing variable)");
		next();
	}

	void parseGlobalVariable()
	{
		if (m_token.type != Lexer::Type::Var) return;
		next();

		std::set<std::string> variables;
		if (m_token.type != Lexer::Type::Identifier) THROW_LOG(Logger::Type::Error, R"(Expected identifier when parsing variable)");
		variables.emplace(m_token.value);
		next();

		while (m_token.type == Lexer::Type::Operator && m_token.value == ",")
		{
			next();
			if (m_token.type != Lexer::Type::Identifier) THROW_LOG(Logger::Type::Error, R"(Expected identifier when parsing variable)");
			if (variables.contains(m_token.value)) LOG(Logger::Type::Warning, "Variable \"" + m_token.value + "\" hides previous declaration of \"" + m_token.value + "\"");
			variables.emplace(m_token.value);
			next();
		}

		if (m_token.type != Lexer::Type::Operator || m_token.value != ":") THROW_LOG(Logger::Type::Error, R"(Expected ':' when parsing variable)");
		next();

		auto variableType = parseVariableType();
		switch (variableType)
		{
		case VariableType::Array:
			break;
		case VariableType::Integer:
			for (const auto& variable : variables)
			{
				if (m_program.globalDeclarations->names.contains(variable)) LOG(Logger::Type::Warning, "Integer variable \"" + m_token.value + "\" hides previous declaration of \"" + m_token.value + "\"");
				globalDeclarations()->names.emplace(variable, GlobalDeclarationTypes::Integer);
				globalDeclarations()->variables.emplace(variable, GlobalInteger());
			}
			break;
		default:
			THROW_LOG(Logger::Type::Error, R"(Unknown VariableType)");
		}
	}

	VariableType parseVariableType()
	{
		if (m_token.type == Lexer::Type::Integer)
		{
			next();
			if (m_token.type != Lexer::Type::Operator || m_token.value != ";") THROW_LOG(Logger::Type::Error, R"(Expected ';' when parsing variable type)");
			next();
			return VariableType::Integer;
		}
		else if (m_token.type == Lexer::Type::Array)
		{
			return parseArray();
		}
		else THROW_LOG(Logger::Type::Error, "(Expected 'array' or 'integer' when parsing variable type)");
	}

	VariableType parseArray()
	{
		return VariableType::Array;
	}

	void parseBlock()
	{
		if (m_token.type != Lexer::Type::Begin) THROW_LOG(Logger::Type::Error, R"(Expected 'begin' when parsing block)");
		next();

		parseReadln();
		parseWriteln();
	}

	void parseReadln()
	{
		if (m_token.type != Lexer::Type::Readln) return;
		next();

		if (m_token.type != Lexer::Type::Operator || m_token.value != "(") THROW_LOG(Logger::Type::Error, R"(Expected '(' when parsing readln)");
		next();

		if (m_token.type != Lexer::Type::Identifier) THROW_LOG(Logger::Type::Error, R"(Expected identifier when parsing readln)");
		auto variable = globalDeclarations()->names.find(m_token.value);
		if (variable == globalDeclarations()->names.end()) THROW_LOG(Logger::Type::Error, R"(Unknown identifier when parsing readln)");
		if (variable->second != GlobalDeclarationTypes::Array && variable->second != GlobalDeclarationTypes::Integer) THROW_LOG(Logger::Type::Error, R"(Function readln takes array or integer)");
		// TODO
		next();

		if (m_token.type != Lexer::Type::Operator || m_token.value != ")") THROW_LOG(Logger::Type::Error, R"(Expected ')' when parsing readln)");
		next();
	}

	void parseWriteln()
	{
		if (m_token.type != Lexer::Type::Writeln) return;
		next();

		if (m_token.type != Lexer::Type::Operator || m_token.value != "(") THROW_LOG(Logger::Type::Error, R"(Expected '(' when parsing readln)");
		next();

		if (m_token.type != Lexer::Type::Identifier) THROW_LOG(Logger::Type::Error, R"(Expected identifier when parsing readln)");
		auto variable = globalDeclarations()->names.find(m_token.value);
		if (variable == globalDeclarations()->names.end()) THROW_LOG(Logger::Type::Error, R"(Unknown identifier when parsing readln)");
		if (variable->second != GlobalDeclarationTypes::Array && variable->second != GlobalDeclarationTypes::Integer) THROW_LOG(Logger::Type::Error, R"(Function readln takes array or integer)");
		// TODO
		next();

		if (m_token.type != Lexer::Type::Operator || m_token.value != ")") THROW_LOG(Logger::Type::Error, R"(Expected ')' when parsing readln)");
		next();
	}

private:
	Lexer m_lexer;
	Program m_program;
	Lexer::Token m_token;
};