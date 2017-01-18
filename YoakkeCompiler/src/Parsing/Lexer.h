#pragma once

#include <map>
#include <string>
#include <vector>
#include "Token.h"

namespace yk
{
	class Lexer
	{
	private:
		friend class Parser;

	private:
		const char* m_Buffer;
		const char* m_Ptr;
		std::map<const std::string, TokenT> m_Lexemes;

	public:
		void AddLexeme(std::string const& l, TokenT tt);
		void SetSource(const char* buf);

		Token Next();
	};
}