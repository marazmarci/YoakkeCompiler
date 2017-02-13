#include "TypeDesc.h"
#include "../Parsing/Token.h"

namespace yk
{
	namespace ast
	{
		TypeDesc::TypeDesc(parse::Position const& pos)
			: Node(pos)
		{
		}

		TypeDesc::~TypeDesc()
		{
		}

		IdentTypeDesc::IdentTypeDesc(parse::Token const& tok)
			: TypeDesc(parse::Position::Get(tok)), Identifier(tok.Value)
		{
		}

		IdentTypeDesc::~IdentTypeDesc()
		{
		}
	}
}