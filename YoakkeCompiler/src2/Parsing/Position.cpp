#include "Position.h"
#include "../Parsing/Token.h"

namespace yk
{
	namespace parse
	{
		Position::Position(ystr const& fn, ysize x, ysize y, ysize ex, ysize ey)
			: File(fn), StartX(x), StartY(y), EndX(ex), EndY(ey)
		{
		}

		Position Position::Get(Token const& tok)
		{
			return Position(tok.File, tok.PosX, tok.PosY, tok.PosX + tok.Value.size(), tok.PosY);
		}

		Position Position::Interval(Position const& a, Position const& b)
		{
			return Position(a.File, a.StartX, a.StartY, b.EndX, b.EndY);
		}
	}
}