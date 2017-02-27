#pragma once

#include "../utility/double_dispatcher.h"
#include "../ast/ast_node.h"
#include "../ast/expr.h"
#include "../ast/stmt.h"
#include "ir_builder.h"

namespace yk {
	class ir_value;
	class ir_type;

	class ir_compiler : public double_dispatcher
		<ir_value*, ast_node,
		expr_stmt, let_expr, bin_expr, preury_expr, postury_expr,
		func_proto, func_expr, body_expr, list_expr,
		int_lit_expr, real_lit_expr, unit_expr> {
	private:
		ir_builder m_Builder;

	public:
		ir_compiler(ir_module& mod);

	public:
		ir_value* dispatch(expr_stmt* stmt) override;
		ir_value* dispatch(bin_expr* exp) override;
		ir_value* dispatch(preury_expr* exp) override;
		ir_value* dispatch(postury_expr* exp) override;
		ir_value* dispatch(func_proto* exp) override;
		ir_value* dispatch(func_expr* exp) override;
		ir_value* dispatch(body_expr* exp) override;
		ir_value* dispatch(list_expr* exp) override;
		ir_value* dispatch(let_expr* exp) override;
		ir_value* dispatch(int_lit_expr* exp) override;
		ir_value* dispatch(real_lit_expr* exp) override;
		ir_value* dispatch(unit_expr* exp) override;

	private:
		ir_type* get_ir_type(type_symbol* ts);
	};
}
