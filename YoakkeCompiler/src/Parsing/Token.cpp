#include "Token.h"

namespace yk
{
	Token::Token(TokenT tt, ystr const& v, ysize r, ysize c)
		: Type(tt), Value(v), Row(r), Column(c - v.size())
	{
	}
}