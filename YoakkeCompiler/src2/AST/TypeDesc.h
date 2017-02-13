#pragma once

#include "Node.h"

namespace yk
{
	namespace ast
	{
		class TypeDesc : public Node
		{
		protected:
			TypeDesc(parse::Position const& pos);

		public:
			virtual ~TypeDesc();
		};

		class IdentTypeDesc : public TypeDesc
		{
		public:
			ystr Identifier;

		public:
			IdentTypeDesc(parse::Token const& tok);
			virtual ~IdentTypeDesc();
		};
	}
}