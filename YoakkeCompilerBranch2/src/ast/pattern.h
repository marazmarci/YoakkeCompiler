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

	class ignore_pattern : public pattern {
	public:
		ignore_pattern(token const& tok);
		virtual ~ignore_pattern();
	};

	class unit_pattern : public pattern {
	public:
		unit_pattern(token const& beg, token const& end);
		virtual ~unit_pattern();
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
		ysptr<pattern> LHS;
		ysptr<pattern> RHS;

	public:
		bin_pattern(token const& op, ysptr<pattern> lhs, ysptr<pattern> rhs);
		virtual ~bin_pattern();
	};

	class list_pattern : public pattern {
	public:
		std::vector<ysptr<pattern>> Elements;

	public:
		list_pattern(ysptr<pattern>& left);
		virtual ~list_pattern();

	public:
		void add(ysptr<pattern>& exp);
	};
}
