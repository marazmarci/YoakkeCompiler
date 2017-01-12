#pragma once

#include "AST.h"

namespace yk
{
	struct TypeDesc : public ASTNode
	{
	public:
		virtual ~TypeDesc() { }
	};
}