#pragma once

#include <cstddef>

namespace yk
{
	class Operator;

	struct NodePos
	{
	public:
		std::size_t StartX;
		std::size_t StartY;
		std::size_t EndX;
		std::size_t EndY;

	public:
		NodePos(std::size_t sx, std::size_t sy, std::size_t ex, std::size_t ey);
	};

	struct OperPos : public NodePos
	{
	public:
		Operator* OP;

	public:
		OperPos(std::size_t sx, std::size_t sy, Operator* o);
	};
}