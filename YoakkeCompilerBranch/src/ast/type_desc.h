#pragma once

#include "ast_node.h"
#include "../parse/token.h"
#include "../utility/optional.h"

namespace yk {
	class type_symbol;

	class type_desc : public ast_node, public visitable<type_desc> {
	public:
		META_BaseVisitable(type_desc)

	public:
		type_symbol* SymbolForm;

	protected:
		type_desc(position const& pos);

	public:
		virtual ~type_desc();
	};

	class ident_type_desc : public type_desc {
	public:
		META_Visitable(ident_type_desc, type_desc)

	public:
		ystr Identifier;

	public:
		ident_type_desc(token const& tok);
		virtual ~ident_type_desc();
	};

	class bin_type_desc : public type_desc {
	public:
		META_Visitable(bin_type_desc, type_desc)

	public:
		type_desc* LHS;
		type_desc* RHS;
		token OP;

	public:
		bin_type_desc(type_desc* l, type_desc* r, token const& o);
		virtual ~bin_type_desc();
	};

	class list_type_desc : public type_desc {
	public:
		META_Visitable(list_type_desc, type_desc)

	public:
		yvec<type_desc*> List;

	public:
		list_type_desc(yvec<type_desc*> const& ls);
		virtual ~list_type_desc();
	};
}