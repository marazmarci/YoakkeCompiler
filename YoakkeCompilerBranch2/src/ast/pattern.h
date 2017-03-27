#pragma once

#include "node.h"
#include "../lexing/token.h"

namespace yk {
	class pattern : public node {
	protected:
		pattern(interval const& pos);

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
		token Operator;
		yshared_ptr<pattern> LHS;
		yshared_ptr<pattern> RHS;

	public:
		bin_pattern(token const& op, yshared_ptr<pattern> lhs, yshared_ptr<pattern> rhs);
		virtual ~bin_pattern();
	};

	class list_pattern : public pattern {
	public:
		std::vector<yshared_ptr<pattern>> Elements;

	public:
		list_pattern(yshared_ptr<pattern>& left);
		virtual ~list_pattern();

	public:
		void add(yshared_ptr<pattern>& exp);
	};
}
