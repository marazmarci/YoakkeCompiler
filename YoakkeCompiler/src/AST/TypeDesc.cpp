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
		: TypeDesc(NodePos::Get(ref)), Identifier(ref.Value)
	{
	}

	IdentTypeDesc::~IdentTypeDesc()
	{
	}

	XMLNode* IdentTypeDesc::ToXML()
	{
		auto node = new XMLNode("IdentTypeDesc");
		node->Attributes.push_back(XMLAttribute("id", Identifier));
		return node;
	}
}