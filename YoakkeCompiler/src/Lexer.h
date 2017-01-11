#pragma once

#include <string>
#include <vector>
#include <map>
#include "Token.h"

namespace yk
{
	typedef std::vector<Token> token_list;

	class Lexer
	{
	private:
		std::map<std::string, std::string> m_Symbols;

	public:
		Lexer();

		token_list Tokenize(const char* src);
		bool AddSymbol(std::string const& val);
		bool AddAlias(std::string const& val, std::string const& alias);
	};
}