#include "ir_compiler.h"
#include "ir_environment.h"
#include "ir_function.h"
#include "ir_instr.h"
#include "ir_basic_block.h"
#include "../semantics/type_symbol.h"
#include "../ast/type_desc.h"

namespace yk {
	ir_compiler::ir_compiler(ir_module& mod)
		: m_Builder(&mod) {
	}

	ir_value* ir_compiler::dispatch(expr_stmt* stmt) {
		return dispatch_gen(stmt->Sub);
	}

	ir_value* ir_compiler::dispatch(bin_expr* exp) {
		if (exp->OP.identifier() == "::") {
			if (auto func = dynamic_cast<func_expr*>(exp->RHS)) {
				dispatch(func);
				return nullptr;
			}
		}
		throw std::exception("UNSUPPORTED OPERATION");
		return nullptr;
	}

	ir_value* ir_compiler::dispatch(preury_expr* exp) {
		return nullptr;
	}
	
	ir_value* ir_compiler::dispatch(postury_expr* exp) {
		return nullptr;
	}

	ir_value* ir_compiler::dispatch(func_proto* exp) {
		yvec<ir_parameter*> params;
		for (auto p : exp->Parameters) {
			if (p->Name.some()) {
				params.push_back(new ir_parameter(p->Name.get().identifier(),
					get_ir_type(p->Type->SymbolForm)));
			}
			else {
				params.push_back(new ir_parameter("",
					get_ir_type(p->Type->SymbolForm)));
			}
		}
		ir_function_proto* proto = new ir_function_proto("temp_func", 
			get_ir_type(exp->ReturnType->SymbolForm), params);
		return proto;
	}

	ir_value* ir_compiler::dispatch(func_expr* exp) {
		auto proto = (ir_function_proto*)dispatch(exp->Prototype);
		auto func = new ir_function(proto);
		m_Builder.add_func(func);
		m_Builder.add_bb(new ir_basic_block("entry"));
		dispatch_gen(exp->Body);
		return func;
	}

	ir_value* ir_compiler::dispatch(body_expr* exp) {
		for (auto sub : exp->Statements) {
			dispatch_gen(sub);
		}
		return nullptr;
	}

	ir_value* ir_compiler::dispatch(list_expr* exp) {
		throw std::exception("MUST NOT BE HIT");
	}

	ir_value* ir_compiler::dispatch(let_expr* exp) {
		auto curr_func = m_Builder.current_func();
		auto curr_block = m_Builder.current_bb();
		m_Builder.set_bb(curr_func->Blocks[0]);
		for (auto pair : exp->Matched) {
			auto ID = pair.first;
			auto EXP = pair.second;
			auto TYPE = EXP->EvalType;

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
		if (!ts) {
			return ir_environment::get_type("void");
		}
		if (auto builtin = dynamic_cast<builtin_type_symbol*>(ts)) {
			if (builtin->Identifier == "unit") {
				return ir_environment::get_type("void");
			}
			return ir_environment::get_type(builtin->Identifier);
		}

		throw std::exception("Unsupported IR type!");
	}
}