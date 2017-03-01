#include "ir_compiler.h"
#include "ir_environment.h"
#include "ir_function.h"
#include "ir_instr.h"
#include "ir_basic_block.h"
#include "../semantics/type_symbol.h"
#include "../ast/type_desc.h"
#include "../semantics/typed_symbol.h"

namespace yk {
	// Generic compiler ///////////////////////////////////////////////////////
	ir_compiler::ir_compiler(ir_module& mod)
		: m_Builder(&mod), m_ExprCompiler(*this), m_StmtCompiler(*this) {
	}

	// Statement compiler /////////////////////////////////////////////////////
	ir_stmt_compiler::ir_stmt_compiler(ir_compiler& b)
		: m_Parent(b), m_Builder(b.m_Builder) {
		META_Visitables(stmt, expr_stmt);
	}

	void ir_stmt_compiler::compile(stmt& st) {
		throw std::exception("Statement IR not handled!");
	}

	void ir_stmt_compiler::compile(expr_stmt& st) {
		auto val = m_Parent.compile(*st.Sub);
		if (auto inst = dynamic_cast<ir_instr*>(val)) {
			m_Builder.add_inst(inst);
		}
		else if (val) {
			throw std::exception("This value can not be an instruction!");
		}
	}

	// Expression compiler ////////////////////////////////////////////////////
	ir_expr_compiler::ir_expr_compiler(ir_compiler& b)
		: m_Parent(b), m_Builder(b.m_Builder) {
		META_Visitables(
			expr,
			ident_expr, unit_expr, int_lit_expr, real_lit_expr, 
			preury_expr, postury_expr, bin_expr,
			mixfix_expr, func_proto, func_expr, body_expr, let_expr);
	}

	ir_value* ir_expr_compiler::compile(expr& exp) {
		throw std::exception("Expression IR not handled!");
	}

	ir_value* ir_expr_compiler::compile(ident_expr& exp) {
		return exp.ValueSymbol->Value;
	}

	ir_value* ir_expr_compiler::compile(unit_expr& exp) {
		return nullptr;
	}

	ir_value* ir_expr_compiler::compile(int_lit_expr& exp) {
		return new ir_int_value(32, exp.Value);
	}

	ir_value* ir_expr_compiler::compile(real_lit_expr& exp) {
		return nullptr;
	}

	ir_value* ir_expr_compiler::compile(preury_expr& exp) {
		return nullptr;
	}
	
	ir_value* ir_expr_compiler::compile(postury_expr& exp) {
		return nullptr;
	}

	ir_value* ir_expr_compiler::compile(bin_expr& exp) {
		if (exp.OP.Identifier() == "::") {
			auto ID_EXP = ((ident_expr*)exp.LHS);
			ystr const& ID = ID_EXP->Identifier;
			if (auto func = dynamic_cast<func_expr*>(exp.RHS)) {
				auto fun = (ir_function*)(*this)(*func);
				fun->Prototype->Name = ID;
				ID_EXP->ValueSymbol->Value = fun->Prototype;
				m_Builder.add_func(fun);
				return nullptr;
			}
			else if (auto prot = dynamic_cast<func_proto*>(exp.RHS)) {
				auto pr = (ir_function_proto*)(*this)(*prot);
				pr->Name = ID;
				ID_EXP->ValueSymbol->Value = pr;
				m_Builder.add_func_proto(pr);
				return nullptr;
			}
		}
		throw std::exception("UNSUPPORTED OPERATION");
	}

	ir_value* ir_expr_compiler::compile(mixfix_expr& exp) {
		if (exp.OP == "call") {
			if (auto func = dynamic_cast<ir_function_proto*>((*this)(*exp.Operands[0]))) {
				// TODO: args
				auto exp_list = exp.Operands[1];
				yvec<expr*> args_e;
				if (auto arg_pre = dynamic_cast<list_expr*>(exp_list)) {
					for (auto a : arg_pre->List) {
						args_e.push_back(a);
					}
				}
				else if (exp_list) {
					args_e.push_back(exp_list);
				}
				
				yvec<ir_value*> val;
				for (auto p : args_e) {
					val.push_back((*this)(*p));
				}

				return new ir_call_instr(func, val);
			}
			else {
				throw std::exception("CALL SANITY");
			}
		}
		else {
			throw std::exception("Mixfix unimplemented!");
		}
	}

	ir_value* ir_expr_compiler::compile(func_proto& exp) {
		yvec<ir_parameter*> params;
		for (auto p : exp.Parameters) {
			if (p->Name.some()) {
				params.push_back(new ir_parameter(p->Name.get().Identifier(),
					get_ir_type(p->Type->SymbolForm)));
			}
			else {
				params.push_back(new ir_parameter("",
					get_ir_type(p->Type->SymbolForm)));
			}
		}
		ir_function_proto* proto = new ir_function_proto("temp_func", 
			get_ir_type(exp.ReturnType->SymbolForm), params);
		return proto;
	}

	ir_value* ir_expr_compiler::compile(func_expr& exp) {
		auto curr_bb = m_Builder.current_bb();
		auto proto = (ir_function_proto*)(*this)(*exp.Prototype);
		auto func = new ir_function(proto);
		m_Builder.set_func(func);
		(*this)(*exp.Body);
		m_Builder.add_inst(new ir_ret_instr());
		m_Builder.set_bb(curr_bb);
		return func;
	}

	ir_value* ir_expr_compiler::compile(body_expr& exp) {
		for (auto sub : exp.Statements) {
			m_Parent.compile(*sub);
		}
		return nullptr;
	}

	ir_value* ir_expr_compiler::compile(let_expr& exp) {
		auto curr_func = m_Builder.current_func();
		auto curr_block = m_Builder.current_bb();
		m_Builder.set_bb(curr_func->Blocks[0]);
		for (auto pair : exp.Matched) {
			auto ID = pair.first;
			auto EXP = pair.second;
			auto TYPE = EXP->EvalType;

			auto all = new ir_alloc_instr(ID, get_ir_type(TYPE));
			m_Builder.add_inst_bb_begin(all);
			if (EXP) {
				m_Builder.add_inst(new ir_store_instr(all, (*this)(*EXP)));
			}
		}
		return nullptr;
	}

	ir_type* ir_expr_compiler::get_ir_type(type_symbol* ts) {
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