#include "ast_expr.h"

AST_expr::AST_expr(interval const& pos, AST_expr_t ty)
	: AST_node(pos), Ty(ty) {
}

AST_expr::~AST_expr() { }

/*****************************************************************************/

AST_block_expr::AST_block_expr(token const& beg,
	yvec<AST_stmt*> const& stmts, yopt<AST_expr*> val,
	token const& end) 
	: AST_expr(interval(beg.Pos, end.Pos), AST_expr_t::Block),
	Statements(stmts), Value(std::move(val)) {
}

AST_block_expr::~AST_block_expr() {
	for (auto& st : Statements) {
		delete st;
	}
	if (Value) {
		delete *Value;
	}
}

/*****************************************************************************/

AST_fn_expr::AST_fn_expr(yopt<token> const& first, 
	yvec<param_t> const& params,
	yopt<AST_ty*> ret, AST_block_expr* body)
	: AST_expr(interval(lparen, body->Pos), 
		AST_expr_t::Fn), 
	Params(params), Return(std::move(ret)), Body(body) {
}

AST_fn_expr::~AST_fn_expr() {
	for (auto& par : Params) {
		delete std::get<1>(par);
	}
	if (Return) {
		delete *Return;
	}
}
