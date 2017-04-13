#pragma once

#include "node.h"
#include "../lexing/token.h"

namespace yk {
	class type_desc : public node {
	protected:
		type_desc(interval const& pos);

	public:
		virtual ~type_desc();
	};

	class unit_type_desc : public type_desc {
	public:
		unit_type_desc(token const& beg, token const& end);
		virtual ~unit_type_desc();
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
		token Operator;
		yshared_ptr<type_desc> LHS;
		yshared_ptr<type_desc> RHS;

	public:
		bin_type_desc(token const& op, yshared_ptr<type_desc> lhs, yshared_ptr<type_desc> rhs);
		virtual ~bin_type_desc();
	};

	class list_type_desc : public type_desc {
	public:
		std::vector<yshared_ptr<type_desc>> Elements;

	public:
		list_type_desc(yshared_ptr<type_desc>& left);
		virtual ~list_type_desc();

	public:
		void add(yshared_ptr<type_desc>& exp);
	};
}
