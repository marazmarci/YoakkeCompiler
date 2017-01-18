#include "Token.h"

namespace yk
{
	Token Token::EPSILON = Token(TokenT::Epsilon, "Epsilon");

	Token::Token(TokenT tt, std::string const& v)
		: Type(tt), Value(v)
	{
	}

	std::ostream& operator<<(std::ostream& stream, Token const& tok)
	{
		switch (tok.Type)
		{
		case TokenT::Epsilon:
			stream << "<Epsilon>";
			break;
		case TokenT::Identifier:
			stream << "<Identifier: '" << tok.Value << "'>";
			break;
		case TokenT::Integer:
			stream << "<Integer: '" << tok.Value << "'>";
			break;
		case TokenT::Keyword:
			stream << "<Keyword: '" << tok.Value << "'>";
			break;
		case TokenT::Operator:
			stream << "<Operator: '" << tok.Value << "'>";
			break;
		case TokenT::Real:
			stream << "<Real: '" << tok.Value << "'>";
			break;
		default:
			stream << "<UNHANDLED>";
		}

		return stream;
	}
}