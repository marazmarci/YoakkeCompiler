#pragma once

#include <iostream>
#include <string>

namespace yk
{
	enum class TokenT
	{
		Epsilon, Ident, Int, Float, Symbol
	};

	struct Token
	{
	public:
		TokenT Type;
		std::string Value;

	public:
		Token(TokenT t, std::string const& v)
			: Type(t), Value(v)
		{
		}

		friend std::ostream& operator<<(std::ostream& stream, Token const& t)
		{
			switch (t.Type)
			{
			case TokenT::Epsilon:
				stream << "Epsilon";
				break;

			case TokenT::Int:
				stream << "Integer(" << t.Value << ")";
				break;

			case TokenT::Float:
				stream << "Float(" << t.Value << ")";
				break;

			case TokenT::Symbol:
				stream << "Symbol - '" << t.Value << "'";
				break;

			case TokenT::Ident:
				stream << "Identifier - '" << t.Value << "'";
				break;
			}

			return stream;
		}
	};
}