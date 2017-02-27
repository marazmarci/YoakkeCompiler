#pragma once

#include "ast_node.h"

namespace yk {
	class pattern : public ast_node {
	protected:
		pattern(position const& pos);

	public:
		virtual ~pattern();
	};

	class skip_pattern : public pattern {
	public:
		skip_pattern(token const& tok);
		virtual ~skip_pattern();
	};

	class ident_pattern : public pattern {
	public:
		ystr Identifier;

	public:
		ident_pattern(token const& tok);
		virtual ~ident_pattern();
	};

	class bin_pattern : public pattern {
	public:
		pattern* LHS;
		pattern* RHS;
		token OP;

	public:
		bin_pattern(pattern* l, pattern* r, token const& o);
		virtual ~bin_pattern();
	};

	class list_pattern : public pattern {
	public:
		yvec<pattern*> List;

	public:
		list_pattern(yvec<pattern*> const& ls);
		virtual ~list_pattern();
	};
}