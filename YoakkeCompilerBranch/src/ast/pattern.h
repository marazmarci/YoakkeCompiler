#pragma once

#include "ast_node.h"

namespace yk {
	class pattern : public ast_node {
	protected:
		pattern(position const& pos);

	public:
		virtual ~pattern();
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

	class enclose_pattern : public pattern {
	public:
		token Begin;
		token End;
		pattern* Sub;

	public:
		enclose_pattern(pattern* s, token const& beg, token const& end);
		virtual ~enclose_pattern();
	};
}