#pragma once

#include "ast_node.h"
#include "../parse/token.h"
#include "../utility/optional.h"

namespace yk {
	class type_desc;
	class param_expr;
	class pattern;
	class stmt;
	class type_symbol;
	class let_meta;
	class typed_symbol;

	class expr : public ast_node, public visitable<expr> {
	public:
		META_BaseVisitable(expr)

	public:
		type_symbol* Hint;
		type_symbol* EvalType;

	protected:
		expr(position const& pos);

	public:
		virtual ~expr();
	};

	class ident_expr : public expr {
	public:
		META_Visitable(ident_expr, expr)

	public:
		const ystr Identifier;
		typed_symbol* ValueSymbol;

	public:
		ident_expr(token const& tok);
		virtual ~ident_expr();
	};

	class unit_expr : public expr {
	public:
		META_Visitable(unit_expr, expr)

	public:
		unit_expr(token const& beg, token const& end);
		virtual ~unit_expr();
	};

	class int_lit_expr : public expr {
	public:
		META_Visitable(int_lit_expr, expr)

	public:
		int Value;

	public:
		int_lit_expr(token const& tok);
		virtual ~int_lit_expr();
	};

	class real_lit_expr : public expr {
	public:
		META_Visitable(real_lit_expr, expr)

	public:
		double Value;

	public:
		real_lit_expr(token const& tok);
		virtual ~real_lit_expr();
	};

	class bin_expr : public expr {
	public:
		META_Visitable(bin_expr, expr)

	public:
		expr* LHS;
		expr* RHS;
		token OP;

	public:
		bin_expr(expr* l, expr* r, token const& o);
		virtual ~bin_expr();
	};

	class preury_expr : public expr {
	public:
		META_Visitable(preury_expr, expr)

	public:
		expr* Sub;
		token OP;

	public:
		preury_expr(expr* s, token const& o);
		virtual ~preury_expr();
	};

	class postury_expr : public expr {
	public:
		META_Visitable(postury_expr, expr)

	public:
		expr* Sub;
		token OP;

	public:
		postury_expr(expr* s, token const& o);
		virtual ~postury_expr();
	};

	class list_expr : public expr {
	public:
		META_Visitable(list_expr, expr)

	public:
		yvec<expr*> List;

	public:
		list_expr(yvec<expr*> const& ls);
		virtual ~list_expr();
	};

	class mixfix_expr : public expr {
	public:
		META_Visitable(mixfix_expr, expr)

	public:
		ystr OP;
		yvec<expr*> Operands;

	public:
		mixfix_expr(ystr const& o, yvec<expr*> const& opers, position const& pos);
		virtual ~mixfix_expr();
	};

	class block_expr : public expr {
	protected:
		block_expr(position const& pos);

	public:
		virtual ~block_expr();
	};

	class func_proto : public expr {
	public:
		META_Visitable(func_proto, expr)

	public:
		yvec<param_expr*> Parameters;
		type_desc* ReturnType;

	public:
		func_proto(token const& beg, position const& end, 
			yvec<param_expr*> const& pars, type_desc* rett);
		virtual ~func_proto();
	};

	class func_expr : public block_expr {
	public:
		META_Visitable(func_expr, expr)

	public:
		func_proto* Prototype;
		expr* Body;

	public:
		func_expr(func_proto* pr, expr* bod);
		virtual ~func_expr();
	};

	class body_expr : public block_expr {
	public:
		META_Visitable(body_expr, expr)

	public:
		yvec<stmt*> Statements;

	public:
		body_expr(token const& beg, token const& end, yvec<stmt*> const& stmts);
		virtual ~body_expr();
	};

	class param_expr : public expr {
	public:
		yopt<token> Name;
		type_desc* Type;

	public:
		param_expr(token const& name, token const& col, type_desc* type);
		param_expr(token const& col, type_desc* type);
		virtual ~param_expr();
	};

	class let_expr : public expr {
	public:
		META_Visitable(let_expr, expr)

	public:
		pattern* Left;
		type_desc* Type;
		expr* Value;
		yvec<ypair<ystr, expr*>> Matched;

	public:
		let_expr(token const& beg, pattern* l, type_desc* t, expr* v);
		virtual ~let_expr();
	};
}