#pragma once

#include "../ast/expr.h"
#include "../utility/double_dispatcher.h"

namespace yk {
	class semantic_checker;
	class symbol_table;

	class expr_checker : public double_dispatcher
		<type_symbol*, expr,
		ident_expr, unit_expr, int_lit_expr, real_lit_expr, bin_expr, preury_expr, 
		postury_expr, enclose_expr, mixfix_expr, func_proto, func_expr, body_expr, 
		param_expr, let_expr> {
	private:
		semantic_checker& m_Checker;
		symbol_table& m_Table;

	public:
		expr_checker(semantic_checker& ch, symbol_table& tab);

	public:
		type_symbol* dispatch(ident_expr* exp) override;
		type_symbol* dispatch(unit_expr* exp) override;
		type_symbol* dispatch(int_lit_expr* exp) override;
		type_symbol* dispatch(real_lit_expr* exp) override;
		type_symbol* dispatch(bin_expr* exp) override;
		type_symbol* dispatch(preury_expr* exp) override;
		type_symbol* dispatch(postury_expr* exp) override;
		type_symbol* dispatch(enclose_expr* exp) override;
		type_symbol* dispatch(mixfix_expr* exp) override;
		type_symbol* dispatch(func_proto* exp) override;
		type_symbol* dispatch(func_expr* exp) override;
		type_symbol* dispatch(body_expr* exp) override;
		type_symbol* dispatch(param_expr* exp) override;
		type_symbol* dispatch(let_expr* exp) override;
	};
}