#pragma once

#include "../ast/expr.h"
#include "../utility/double_dispatcher.h"

namespace yk {
	class semantic_checker;

	class expr_checker : public double_dispatcher
		<void, expr,
		ident_expr, unit_expr, bin_expr, preury_expr, postury_expr, enclose_expr,
		mixfix_expr, func_proto, func_expr, body_expr, param_expr, let_expr> {
	private:
		semantic_checker* m_Checker;

	public:
		expr_checker(semantic_checker* ch);

	public:
		void dispatch(ident_expr* exp) override;
		void dispatch(unit_expr* exp) override;
		void dispatch(bin_expr* exp) override;
		void dispatch(preury_expr* exp) override;
		void dispatch(postury_expr* exp) override;
		void dispatch(enclose_expr* exp) override;
		void dispatch(mixfix_expr* exp) override;
		void dispatch(func_proto* exp) override;
		void dispatch(func_expr* exp) override;
		void dispatch(body_expr* exp) override;
		void dispatch(param_expr* exp) override;
		void dispatch(let_expr* exp) override;
	};
}