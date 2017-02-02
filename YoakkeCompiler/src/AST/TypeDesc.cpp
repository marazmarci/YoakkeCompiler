#include "TypeDesc.h"

namespace yk
{
	TypeDesc::TypeDesc(NodePos const& p)
		: Node(p), SymbolForm(nullptr)
	{
	}

	TypeDesc::~TypeDesc()
	{
	}

	IdentTypeDesc::IdentTypeDesc(Token const& ref)
		: TypeDesc(NodePos(ref.Column, ref.Row, ref.Column + ref.Value.size(), ref.Row)), 
		Identifier(ref.Value)
	{
	}

	IdentTypeDesc::~IdentTypeDesc()
	{
	}

	std::string IdentTypeDesc::ToXML()
	{
		return "<IdentTypeDesc id=\"" + Identifier + "\"/>";
	}
}