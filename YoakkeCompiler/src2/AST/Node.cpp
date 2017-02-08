#include "Node.h"

namespace yk
{
	namespace ast
	{
		Node::Node(parse::Position const& p)
			: Pos(p)
		{
		}

		Node::~Node()
		{
		}
	}
}