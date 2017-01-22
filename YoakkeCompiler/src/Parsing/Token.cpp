#include "Token.h"

namespace yk
{
	Token::Token(TokenT tt, std::string const& v, std::size_t r, std::size_t c)
		: Type(tt), Value(v), Row(r), Column(c)
	{
	}
}