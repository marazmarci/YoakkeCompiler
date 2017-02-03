#pragma once

#include <cstddef>
#include "../Types.h"

namespace yk
{
	class Token;
	class Expr;

	class NodePos
	{
	public:
		ysize StartX;
		ysize StartY;
		ysize EndX;
		ysize EndY;

	public:
		NodePos(ysize sx, ysize sy, ysize ex, ysize ey);

	public:
		static NodePos Get(Token const& tok);
		static NodePos Interval(Token const& beg, Token const& end);
		static NodePos Join(Expr* a, Expr* b);
	};
}