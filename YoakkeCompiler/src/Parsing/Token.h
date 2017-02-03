#pragma once

#include <cstddef>
#include <iostream>
#include <string>
#include "../Types.h"

namespace yk
{
	enum class TokenT : std::uint8_t
	{
		Epsilon, Unknown, Identifier, Keyword, Operator, Integer, Real
	};

	class Token
	{
	public:
		ystr Value;
		ysize Row;
		ysize Column;
		TokenT Type;

	public:
		Token(TokenT tt, ystr const& v, ysize r, ysize c);
	};
}