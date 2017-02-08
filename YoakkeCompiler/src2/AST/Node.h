#pragma once

#include "../Parsing/Position.h"

namespace yk
{
	namespace ast
	{
		// An abstract Node class for the AST
		class Node
		{
		public:
			const parse::Position Pos;

		protected:
			Node(parse::Position const& p);

		public:
			virtual ~Node();
		};
	}
}