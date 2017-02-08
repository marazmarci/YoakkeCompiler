#include "Token.h"
#include "../Debug/Logger.h"

namespace yk
{
	namespace parse
	{
		Token::Token(TokenT tt, ystr const& val, ysize x, ysize y, ystr const& fn)
			: Type(tt), Value(val), PosX(x), PosY(y), File(fn)
		{
		}

		std::ostream& operator<<(std::ostream& os, Token const& t)
		{
			switch (t.Type)
			{
			case TokenT::Epsilon:
				os << "EPSILON";
				break;

			case TokenT::Identifier:
				os << "IDENTIFIER";
				break;

			case TokenT::Integer:
				os << "INTEGER";
				break;

			case TokenT::Keyword:
				os << "KEYWORD";
				break;

			case TokenT::Real:
				os << "REAL";
				break;

			case TokenT::Symbol:
				os << "SYMBOL";
				break;

			default:
				//dbg::Logger::STATIC_ERROR() << "Unhandled token type for: "
				//	<< "'operator<<()'!" << dbg::endlog;
				return os;
			}
			os << " - '" << t.Value << "'";
			return os;
		}
	}
}