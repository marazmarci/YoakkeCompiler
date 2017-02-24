#pragma once

#include "ast_node.h"

namespace yk {
	class type_desc : public ast_node {
	protected:
		type_desc(position const& pos);

	public:
		virtual ~type_desc();
	};

	class ident_type_desc : public type_desc {
	public:
		ystr Identifier;

	public:
		ident_type_desc(token const& tok);
		virtual ~ident_type_desc();
	};
}