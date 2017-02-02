#pragma once

#include <string>
#include "../Parsing/NodePos.h"

namespace yk
{
	class Node
	{
	public:
		NodePos Position;

	public:
		Node(NodePos const& p);
		virtual ~Node();

	public:
		virtual std::string ToXML() = 0;
	};
}