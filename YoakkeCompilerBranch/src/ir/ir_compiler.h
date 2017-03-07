#pragma once

#include "../utility/visitor.h"
#include "../ast/ast_node.h"
#include "../ast/expr.h"
#include "../ast/stmt.h"
#include "ir_builder.h"

namespace yk {
	class ir_value;
	class ir_type;
	class ir_compiler;

	class ir_stmt_compiler : public visitor<stmt> {
	public:
		META_Visitor(ir_stmt_compiler, compile)

	private:
		ir_compiler& m_Parent;
		ir_builder& m_Builder;

	public:
		ir_stmt_compiler(ir_compiler& b);

	public:
		void compile(stmt& st);
		void compile(expr_stmt& st);
	};

	class ir_expr_compiler : public visitor<expr, ir_value*> {
	public:
		META_Visitor(ir_expr_compiler, compile)
	
	private:
		ir_compiler& m_Parent;
		ir_builder& m_Builder;

	public:
		ir_expr_compiler(ir_compiler& b);

	public:
		ir_value* compile(expr& exp);
		ir_value* compile(ident_expr& exp);
		ir_value* compile(unit_expr& exp);
		ir_value* compile(int_lit_expr& exp);
		ir_value* compile(real_lit_expr& exp);
		ir_value* compile(preury_expr& exp);
		ir_value* compile(postury_expr& exp);
		ir_value* compile(bin_expr& exp);
		ir_value* compile(mixfix_expr& exp);
		ir_value* compile(func_proto& exp);
		ir_value* compile(func_expr& exp);
		ir_value* compile(body_expr& exp);
		ir_value* compile(let_expr& exp);
		ir_value* compile(const_asgn_expr& exp);

	private:
		ir_type* get_ir_type(type_symbol* ts);
	};

	class ir_compiler {
	private:
		friend class ir_expr_compiler;
		friend class ir_stmt_compiler;

	private:
		ir_stmt_compiler m_StmtCompiler;
		ir_expr_compiler m_ExprCompiler;
		ir_builder m_Builder;

	public:
		ir_compiler(ir_module& mod);

	public:
		inline ir_value* compile(expr& exp) {
			return m_ExprCompiler(exp);
		}

		inline void compile(stmt& st) {
			m_StmtCompiler(st);
		}
	};
}
