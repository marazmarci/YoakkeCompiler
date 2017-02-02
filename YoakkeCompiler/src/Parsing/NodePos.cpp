#include "NodePos.h"
#include "../Parsing/Operator.h"

namespace yk
{
	NodePos::NodePos(std::size_t sx, std::size_t sy, std::size_t ex, std::size_t ey)
		: StartX(sx), StartY(sy), EndX(ex), EndY(ey)
	{
	}

	OperPos::OperPos(std::size_t sx, std::size_t sy, Operator* o)
		: NodePos(sx, sy, sx + o->Symbol.size(), sy), OP(o)
	{
	}
}