#pragma once

#include "../Common.h"

namespace yk
{
	namespace parse
	{
		class Token;

		class Position
		{
		public:
			const ystr File;
			const ysize StartX;
			const ysize StartY;
			const ysize EndX;
			const ysize EndY;

		public:
			Position(ystr const& fn, ysize x, ysize y, ysize ex, ysize ey);

		public:
			static Position Get(Token const& tok);
			static Position Interval(Position const& a, Position const& b);
		};
	}
}