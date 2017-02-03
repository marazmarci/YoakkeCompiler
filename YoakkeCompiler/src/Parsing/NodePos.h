#pragma once

#include <cstddef>
#include "../Types.h"

namespace yk
{
	class Token;
	class Node;

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
		static NodePos Interval(NodePos const& beg, NodePos const& end);
		static NodePos Join(Node* a, Node* b);
	};
}