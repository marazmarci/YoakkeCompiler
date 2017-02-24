#pragma once

#include "ast_node.h"
#include "../parse/token.h"

namespace yk {
	class type_desc;

	class expr : public ast_node {
	protected:
		expr(position const& pos);

	public:
		virtual ~expr();
	};

	class ident_expr : public expr {
	public:
		const ystr identifier;

	public:
		ident_expr(token const& tok);
		virtual ~ident_expr();
	};

	class bin_expr : public expr {
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
		expr* Sub;
		token OP;

	public:
		preury_expr(expr* s, token const& o);
		virtual ~preury_expr();
	};

	class postury_expr : public expr {
	public:
		expr* Sub;
		token OP;

	public:
		postury_expr(expr* s, token const& o);
		virtual ~postury_expr();
	};

	class enclose_expr : public expr {
	public:
		token Begin;
		token End;
		expr* Sub;

	public:
		enclose_expr(expr* s, token const& beg, token const& end);
		virtual ~enclose_expr();
	};

	class mixfix_expr : public expr {
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

	class func_proto : public block_expr {
	private:
		typedef ypair<token, type_desc*> param_t;

	public:
		yvec<param_t> Parameters;
		type_desc* ReturnType;

	public:
		func_proto(token const& beg, position const& end, yvec<param_t> const& pars, type_desc* rett);
		virtual ~func_proto();
	};

	class func_expr : public expr {
	public:
		func_proto* Prototype;
		expr* Body;

	public:
		func_expr(func_proto* pr, expr* bod);
		virtual ~func_expr();
	};
}