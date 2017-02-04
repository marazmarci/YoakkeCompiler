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

	FuncTypeDesc::FuncTypeDesc(yvec<TypeDesc*> const& pars, TypeDesc* rt, NodePos const& p)
		: TypeDesc(p), Parameters(pars), ReturnType(rt)
	{
	}

	FuncTypeDesc::~FuncTypeDesc()
	{
		for (auto p : Parameters) delete p;
		delete ReturnType;
	}

	XMLNode* FuncTypeDesc::ToXML()
	{
		auto node = new XMLNode("FunctionTypeDesc");
		for (auto p : Parameters) node->Children.push_back(p->ToXML());
		node->Children.push_back(ReturnType->ToXML());
		return node;
	}
}