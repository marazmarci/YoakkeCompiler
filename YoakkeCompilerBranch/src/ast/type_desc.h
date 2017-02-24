#pragma once

#include "ast_node.h"
#include "../parse/token.h"
#include "../utility/optional.h"

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

	class bin_type_desc : public type_desc {
	public:
		type_desc* LHS;
		type_desc* RHS;
		token OP;

	public:
		bin_type_desc(type_desc* l, type_desc* r, token const& o);
		virtual ~bin_type_desc();
	};

	class enclose_type_desc : public type_desc {
	public:
		token Begin;
		token End;
		type_desc* Sub;

	public:
		enclose_type_desc(type_desc* s, token const& beg, token const& end);
		virtual ~enclose_type_desc();
	};
}