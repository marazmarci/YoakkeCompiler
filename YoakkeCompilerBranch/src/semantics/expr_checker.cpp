#include "expr_checker.h"
#include "symbol_table.h"
#include "typed_symbol.h"
#include "semantic_checker.h"
#include "let_pattern.h"

namespace yk {
	expr_checker::expr_checker(semantic_checker& ch, symbol_table& tab)
		: m_Checker(ch), m_Table(tab) {
	}

	type_symbol* expr_checker::dispatch(ident_expr* exp) {
		auto syms = m_Table.ref(exp->identifier);
		auto typed_set = symbol_table::filter<typed_symbol>(syms);
		if (exp->Hint) {
			typed_set = m_Table.filter_typed_match(typed_set, exp->Hint);
		}
		if (typed_set.size() == 0) {
			throw std::exception("Identifier is undefined!");
		}
		else if (typed_set.size() == 1) {
			return typed_set[0]->Type;
		}
		else {
			throw std::exception("Identifier is ambigous!");
		}
	}

	type_symbol* expr_checker::dispatch(unit_expr* exp) {
		return reinterpret_cast<type_symbol*>(symbol_table::UNIT);
	}

	type_symbol* expr_checker::dispatch(int_lit_expr* exp) {
		return reinterpret_cast<type_symbol*>(symbol_table::INT32);
	}

	type_symbol* expr_checker::dispatch(real_lit_expr* exp) {
		return reinterpret_cast<type_symbol*>(symbol_table::FLOAT32);
	}

	type_symbol* expr_checker::dispatch(bin_expr* exp) {
		if (exp->OP.identifier() == "::") {
			ident_expr* ident = reinterpret_cast<ident_expr*>(exp->LHS);
			auto rhs_type = dispatch_gen(exp->RHS);
			auto sym_set = m_Table.ref(ident->identifier);
			auto typed_set = symbol_table::filter<typed_symbol>(sym_set);
			typed_set = symbol_table::filter_typed_match(typed_set, rhs_type);
			if (typed_set.size()) {
				throw std::exception("Ambigious constant binding is not allowed!");
			}
			else {
				m_Table.decl(new const_bind_symbol(ident->identifier, rhs_type, exp->RHS));
				return symbol_table::UNIT;
			}
		}
		else if (exp->OP.identifier() == "=") {
			auto lval = dispatch_gen(exp->LHS); // TODO: check if lvalue
			auto rval = dispatch_gen(exp->RHS);
			if (lval) {
				if (!rval) {
					throw std::exception("Use of undeduced type!");
				}
				if (lval->match(rval)) {
					return lval;
				}
				else {
					throw std::exception("Assignment type mismatch!");
				}
			}
			else {
				throw std::exception("Type inference not implemented!");
			}
		}
		else {
			// Create pseudo-function to match
			yvec<type_symbol*> args = {dispatch_gen(exp->LHS), dispatch_gen(exp->RHS)};
			auto pseudo_func = new func_type_symbol(args, nullptr);
			auto syms = m_Table.ref("bin_op" + exp->OP.identifier());
			auto typed_set = symbol_table::filter<const_bind_symbol>(syms);
			typed_set = m_Table.filter_typed_match(typed_set, pseudo_func);
			if (typed_set.size() == 0) {
				throw std::exception("Undefined operator (bin)!");
			}
			else if (typed_set.size() == 1) {
				auto result_sym = typed_set[0];
				if (auto func_sym = dynamic_cast<func_type_symbol*>(result_sym->Type)) {
					return func_sym->ReturnType;
				}
				else {
					throw std::exception("SANITY ERROR");
				}
			}
			else {
				throw std::exception("Ambigous operator!");
			}
		}
	}

	type_symbol* expr_checker::dispatch(preury_expr* exp) {
		yvec<type_symbol*> args = { dispatch_gen(exp->Sub) };
		auto pseudo_func = new func_type_symbol(args, nullptr);
		auto syms = m_Table.ref("pre_op" + exp->OP.identifier());
		auto typed_set = symbol_table::filter<const_bind_symbol>(syms);
		typed_set = m_Table.filter_typed_match(typed_set, pseudo_func);
		if (typed_set.size() == 0) {
			throw std::exception("Undefined operator (pre ury)!");
		}
		else if (typed_set.size() == 1) {
			auto result_sym = typed_set[0];
			if (auto func_sym = dynamic_cast<func_type_symbol*>(result_sym->Type)) {
				return func_sym->ReturnType;
			}
			else {
				throw std::exception("SANITY ERROR");
			}
		}
		else {
			throw std::exception("Ambigous operator!");
		}
	}

	type_symbol* expr_checker::dispatch(postury_expr* exp) {
		yvec<type_symbol*> args = { dispatch_gen(exp->Sub) };
		auto pseudo_func = new func_type_symbol(args, nullptr);
		auto syms = m_Table.ref("post_op" + exp->OP.identifier());
		auto typed_set = symbol_table::filter<const_bind_symbol>(syms);
		typed_set = m_Table.filter_typed_match(typed_set, pseudo_func);
		if (typed_set.size() == 0) {
			throw std::exception("Undefined operator (post ury)!");
		}
		else if (typed_set.size() == 1) {
			auto result_sym = typed_set[0];
			if (auto func_sym = dynamic_cast<func_type_symbol*>(result_sym->Type)) {
				return func_sym->ReturnType;
			}
			else {
				throw std::exception("SANITY ERROR");
			}
		}
		else {
			throw std::exception("Ambigous operator!");
		}
	}

	type_symbol* expr_checker::dispatch(list_expr* exp) {
		yvec<type_symbol*> types;
		for (auto el : exp->List) {
			types.push_back(dispatch_gen(el));
		}
		return new tuple_type_symbol(types);
	}

	type_symbol* expr_checker::dispatch(mixfix_expr* exp) {
		if (exp->OP == "call") {
			auto func_exp = exp->Operands[0];
			yvec<type_symbol*> parameters;
			if (exp->Operands.size() == 2) {
				auto args = exp->Operands[1];
				if (auto arg_list = dynamic_cast<list_expr*>(args)) {
					for (auto arg : arg_list->List) {
						parameters.push_back(dispatch_gen(arg));
					}
				}
				else {
					parameters.push_back(dispatch_gen(args));
				}
			}
			// Pseudo-function
			auto pseudo_func = new func_type_symbol(parameters, nullptr);
			func_exp->Hint = pseudo_func;
			auto func_t = dispatch_gen(func_exp);
			if (func_t) {
				if (auto func_ts = dynamic_cast<func_type_symbol*>(func_t)) {
					return func_ts->ReturnType;
				}
				else {
					throw std::exception("Cannot call non-function values!");
				}
			}
			else {
				throw std::exception("Cannot call non-function values!");
			}
		}
		else {
			throw std::exception("Unimplemented mixfix operator!");
		}
	}

	type_symbol* expr_checker::dispatch(func_proto* exp) {
		yvec<type_symbol*> args;
		for (auto arg : exp->Parameters) {
			args.push_back(m_Checker.check_type(arg->Type));
		}
		auto rett = m_Checker.check_type(exp->ReturnType);
		return new func_type_symbol(args, rett);
	}

	type_symbol* expr_checker::dispatch(func_expr* exp) {
		m_Table.push(new scope());
		auto protot = dispatch(exp->Prototype);
		yset<ystr> arg_names;
		for (auto arg : exp->Prototype->Parameters) {
			if (arg->Name.some()) {
				auto arg_n = arg->Name.get().identifier();
				if (arg_names.find(arg_n) != arg_names.end()) {
					throw std::exception("Parameter already defined!");
				}
				arg_names.insert(arg_n);
			}
			else {
				// TODO: WARN
			}
		}
		m_Checker.check_expr(exp->Body);
		m_Table.pop();
		
		return protot;
	}

	type_symbol* expr_checker::dispatch(body_expr* exp) {
		m_Checker.check(exp->Statements);

		// TODO
		return nullptr;
	}

	type_symbol* expr_checker::dispatch(param_expr* exp) {
		// TODO
		return nullptr;
	}

	type_symbol* expr_checker::dispatch(let_expr* exp) {
		type_symbol* hintt = nullptr;
		type_symbol* rett = nullptr;
		if (exp->Type) hintt = m_Checker.check_type(exp->Type);
		if (exp->Value) {
			exp->Value->Hint = hintt;
			rett = dispatch_gen(exp->Value);
			if (!rett) {
				throw std::exception("Use of undeduced type!");
			}
			if (hintt) {
				if (!rett->same(hintt)) {
					throw std::exception("Wrong type for let!");
				}
			}
		}
		let_pat::define(m_Table, exp->Left, rett, exp->Value, exp->Meta);
		return symbol_table::UNIT;
	}
}