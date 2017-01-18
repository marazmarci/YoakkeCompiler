#include "TypeDesc.h"

namespace yk
{
	IdentTypeDesc::IdentTypeDesc(std::string const& id)
		: Identifier(id)
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