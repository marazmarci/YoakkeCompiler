#pragma once

#include "Position.h"
#include "../Common.h"

namespace yk
{
	namespace parse
	{
		enum class TokenT
		{
			Epsilon, Symbol, Keyword, Identifier, Integer, Real
		};

		class Token
		{
		public:
			TokenT Type;
			ystr Value;
			ysize PosX;
			ysize PosY;
			ystr File;

		public:
			Token(TokenT tt, ystr const& val, ysize x, ysize y, ystr const& fn);
		};

		std::ostream& operator<<(std::ostream& os, Token const& t);
	}
}