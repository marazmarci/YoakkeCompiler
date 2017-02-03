#pragma once

#include <string>
#include "../Parsing/NodePos.h"
#include "../Utils/XMLNode.h"

namespace yk
{
	// Abstract AST node
	// Children must override ToXML, which is an XML dump of the subtree
	class Node
	{
	public:
		NodePos Position;

	public:
		Node(NodePos const& p);
		virtual ~Node();

	public:
		virtual XMLNode* ToXML() = 0;
	};
}