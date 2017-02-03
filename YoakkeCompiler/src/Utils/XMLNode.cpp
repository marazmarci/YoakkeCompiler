#include "XMLNode.h"

namespace yk
{
	XMLAttribute::XMLAttribute(ystr const& n, ystr const& v)
		: Name(n), Value(v)
	{
	}

	XMLAttribute::~XMLAttribute()
	{
	}

	XMLNode::XMLNode(ystr const& name)
		: Name(name)
	{
	}

	XMLNode::~XMLNode()
	{
		for (auto c : Children) delete c;
	}
}