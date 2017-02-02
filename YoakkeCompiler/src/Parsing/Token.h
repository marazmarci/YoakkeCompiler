#pragma once

#include <cstddef>
#include <iostream>
#include <string>
#include "NodePos.h"

namespace yk
{
	enum class TokenT : std::uint8_t
	{
		Epsilon, Unknown, Identifier, Keyword, Operator, Integer, Real
	};

	class Token
	{
	public:
		std::string Value;
		std::size_t Row;
		std::size_t Column;
		TokenT Type;

	public:
		Token(TokenT tt, std::string const& v, std::size_t r, std::size_t c);
		NodePos GetPos() const;
	};
}