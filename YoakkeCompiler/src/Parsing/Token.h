#pragma once

#include <iostream>
#include <string>

namespace yk
{
	enum class TokenT
	{
		Epsilon, Unknown, Identifier, Keyword, Operator, Integer, Real
	};

	class Token
	{
	public:
		TokenT Type;
		std::string Value;
		std::size_t Row;
		std::size_t Column;

	public:
		Token(TokenT tt, std::string const& v, std::size_t r, std::size_t c);
	};
}