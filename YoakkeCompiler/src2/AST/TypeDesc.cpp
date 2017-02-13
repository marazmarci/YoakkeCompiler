#include "TypeDesc.h"

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
	}
}