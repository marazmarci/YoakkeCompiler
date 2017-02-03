#include "NodePos.h"
#include "Token.h"
#include "../AST/Expr.h"

namespace yk
{
	NodePos::NodePos(ysize sx, ysize sy, ysize ex, ysize ey)
		: StartX(sx), StartY(sy), EndX(ex), EndY(ey)
	{
	}

	NodePos NodePos::Get(Token const& tok)
	{
		return NodePos(tok.Column, tok.Row, tok.Column + tok.Value.size(), tok.Row);
	}

	NodePos NodePos::Interval(Token const& beg, Token const& end)
	{
		auto bp = Get(beg);
		auto ep = Get(end);
		return NodePos(bp.StartX, bp.StartY, ep.EndX, ep.EndY);
	}

	NodePos NodePos::Join(Expr* a, Expr* b)
	{
		auto& apos = a->Position;
		auto& bpos = b->Position;

		auto* first = &apos;
		auto* second = &bpos;

		if ((bpos.EndY < apos.StartY) ||
			(bpos.EndY == apos.StartY && bpos.EndX <= apos.StartX))
		{
			first = &bpos;
			second = &apos;
		}

		return NodePos(first->StartX, first->StartY, second->EndX, second->EndY);
	}
}