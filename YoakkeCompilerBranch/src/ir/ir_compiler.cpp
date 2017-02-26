#include "ir_compiler.h"
#include "ir_environment.h"
#include "ir_function.h"
#include "ir_instr.h"
#include "../semantics/type_symbol.h"

namespace yk {
	ir_compiler::ir_compiler()
		: m_Builder(ir_environment::CurrentModule) {
	}

	ir_value* ir_compiler::dispatch(expr_stmt* stmt) {
		dispatch_gen(stmt->Sub);
	}

	ir_value* ir_compiler::dispatch(bin_expr* exp) {
		return nullptr;
	}

	ir_value* ir_compiler::dispatch(preury_expr* exp) {
		return nullptr;
	}
	
	ir_value* ir_compiler::dispatch(postury_expr* exp) {
		return nullptr;
	}

	ir_value* ir_compiler::dispatch(func_expr* exp) {
		return nullptr;
	}

	ir_value* ir_compiler::dispatch(body_expr* exp) {
		return nullptr;
	}

	ir_value* ir_compiler::dispatch(enclose_expr* exp) {
		return nullptr;
	}

	ir_value* ir_compiler::dispatch(let_expr* exp) {
		auto curr_func = m_Builder.current_func();
		auto curr_block = m_Builder.current_bb();
		m_Builder.set_bb(curr_func->Blocks[0]);
		for (auto pair : exp->Meta.Values) {
			auto ID = std::get<0>(pair);
			auto EXP = std::get<1>(pair);
			auto TYPE = std::get<2>(pair);

			m_Builder.add_inst(new ir_alloc_instr(ID, get_ir_type(TYPE)));
		}
		return nullptr;
	}

	ir_value* ir_compiler::dispatch(int_lit_expr* exp) {
		return nullptr;
	}

	ir_value* ir_compiler::dispatch(real_lit_expr* exp) {
		return nullptr;
	}

	ir_value* ir_compiler::dispatch(unit_expr* exp) {
		return nullptr;
	}

	ir_type* ir_compiler::get_ir_type(type_symbol* ts) {
		if (auto builtin = dynamic_cast<builtin_type_symbol*>(ts)) {
			return ir_environment::get_type(builtin->Identifier);
		}

		throw std::exception("Unsupported IR type!");
	}
}