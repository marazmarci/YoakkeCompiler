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
		static Token EPSILON;

	public:
		TokenT Type;
		std::string Value;

	public:
		Token(TokenT tt, std::string const& v);

	public:
		friend std::ostream& operator<<(std::ostream& stream, Token const& tok);
	};
}