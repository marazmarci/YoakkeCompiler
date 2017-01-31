#include "TypeDesc.h"

namespace yk
{
	TypeDesc::TypeDesc()
		: SymbolForm(nullptr)
	{
	}

	IdentTypeDesc::IdentTypeDesc(std::string const& id, Token const& ref)
		: Identifier(id), Reference(ref)
	{
	}

	IdentTypeDesc::~IdentTypeDesc()
	{
	}

	std::string IdentTypeDesc::ToString()
	{
		return "<IdentTypeDesc id=\"" + Identifier + "\"/>";
	}
}