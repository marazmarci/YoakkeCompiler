#include "expr_checker.h"
#include "symbol_table.h"
#include "typed_symbol.h"
#include "semantic_checker.h"
#include "let_pattern.h"

namespace yk {
	expr_checker::expr_checker(semantic_checker& ch, symbol_table& tab)
		: m_Checker(ch), m_Table(tab) {
		META_Visitables(expr,
			ident_expr, unit_expr, int_lit_expr, real_lit_expr, bin_expr, preury_expr,
			postury_expr, list_expr, mixfix_expr, func_proto, func_expr, body_expr,
			param_expr, let_expr);
	}

	type_symbol* expr_checker::check(expr& exp) {
		throw std::exception("Expression checking not implemented!");
	}

	type_symbol* expr_checker::check(ident_expr& exp) {
		auto syms = m_Table.ref(exp.Identifier);
		auto typed_set = symbol_table::filter<typed_symbol>(syms);
		if (exp.Hint) {
			typed_set = m_Table.filter_typed_match(typed_set, exp.Hint);
		}
		if (typed_set.size() == 0) {
			throw std::exception("Identifier is undefined!");
		}
		else {
			// Always choose the last one (shadowing previous)
			exp.ValueSymbol = typed_set[typed_set.size() - 1];
			exp.EvalType = typed_set[typed_set.size() - 1]->Type;
			return exp.EvalType;
		}
	}

	type_symbol* expr_checker::check(unit_expr& exp) {
		exp.EvalType = reinterpret_cast<type_symbol*>(symbol_table::UNIT);
		return exp.EvalType;
	}

	type_symbol* expr_checker::check(int_lit_expr& exp) {
		exp.EvalType = reinterpret_cast<type_symbol*>(symbol_table::INT32);
		return exp.EvalType;
	}

	type_symbol* expr_checker::check(real_lit_expr& exp) {
		exp.EvalType = reinterpret_cast<type_symbol*>(symbol_table::FLOAT32);
		return exp.EvalType;
	}

	type_symbol* expr_checker::check(bin_expr& exp) {
		if (exp.OP.Identifier() == "::") {
			ident_expr* ident = reinterpret_cast<ident_expr*>(exp.LHS);
			auto rhs_type = (*this)(*exp.RHS);
			auto sym_set = m_Table.ref(ident->Identifier);
			auto typed_set = symbol_table::filter<typed_symbol>(sym_set);
			typed_set = symbol_table::filter_typed_match(typed_set, rhs_type);
			if (typed_set.size()) {
				throw std::exception("Ambigious constant binding is not allowed!");
			}
			else {
				auto cbs = new const_bind_symbol(ident->Identifier, rhs_type, exp.RHS);
				m_Table.decl(cbs);
				exp.EvalType = symbol_table::UNIT;
				ident->ValueSymbol = cbs;
				return exp.EvalType;
			}
		}
		else if (exp.OP.Identifier() == "=") {
			auto lval = (*this)(*exp.LHS); // TODO: check if lvalue
			auto rval = (*this)(*exp.RHS);
			if (lval) {
				if (!rval) {
					throw std::exception("Use of undeduced type!");
				}
				if (lval->match(rval)) {
					exp.EvalType = lval;
					return exp.EvalType;
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
			yvec<type_symbol*> args = {(*this)(*exp.LHS), (*this)(*exp.RHS)};
			auto pseudo_func = new func_type_symbol(args, nullptr);
			auto syms = m_Table.ref("bin_op" + exp.OP.Identifier());
			auto typed_set = symbol_table::filter<const_bind_symbol>(syms);
			typed_set = m_Table.filter_typed_match(typed_set, pseudo_func);
			if (typed_set.size() == 0) {
				throw std::exception("Undefined operator (bin)!");
			}
			else if (typed_set.size() == 1) {
				auto result_sym = typed_set[0];
				if (auto func_sym = dynamic_cast<func_type_symbol*>(result_sym->Type)) {
					exp.EvalType = func_sym->ReturnType;
					return exp.EvalType;
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

	type_symbol* expr_checker::check(preury_expr& exp) {
		yvec<type_symbol*> args = { (*this)(*exp.Sub) };
		auto pseudo_func = new func_type_symbol(args, nullptr);
		auto syms = m_Table.ref("pre_op" + exp.OP.Identifier());
		auto typed_set = symbol_table::filter<const_bind_symbol>(syms);
		typed_set = m_Table.filter_typed_match(typed_set, pseudo_func);
		if (typed_set.size() == 0) {
			throw std::exception("Undefined operator (pre ury)!");
		}
		else if (typed_set.size() == 1) {
			auto result_sym = typed_set[0];
			if (auto func_sym = dynamic_cast<func_type_symbol*>(result_sym->Type)) {
				exp.EvalType = func_sym->ReturnType;
				return exp.EvalType;
			}
			else {
				throw std::exception("SANITY ERROR");
			}
		}
		else {
			throw std::exception("Ambigous operator!");
		}
	}

	type_symbol* expr_checker::check(postury_expr& exp) {
		yvec<type_symbol*> args = { (*this)(*exp.Sub) };
		auto pseudo_func = new func_type_symbol(args, nullptr);
		auto syms = m_Table.ref("post_op" + exp.OP.Identifier());
		auto typed_set = symbol_table::filter<const_bind_symbol>(syms);
		typed_set = m_Table.filter_typed_match(typed_set, pseudo_func);
		if (typed_set.size() == 0) {
			throw std::exception("Undefined operator (post ury)!");
		}
		else if (typed_set.size() == 1) {
			auto result_sym = typed_set[0];
			if (auto func_sym = dynamic_cast<func_type_symbol*>(result_sym->Type)) {
				exp.EvalType = func_sym->ReturnType;
				return exp.EvalType;
			}
			else {
				throw std::exception("SANITY ERROR");
			}
		}
		else {
			throw std::exception("Ambigous operator!");
		}
	}

	type_symbol* expr_checker::check(list_expr& exp) {
		yvec<type_symbol*> types;
		for (auto el : exp.List) {
			types.push_back((*this)(*el));
		}
		exp.EvalType = new tuple_type_symbol(types);
		return exp.EvalType;
	}

	type_symbol* expr_checker::check(mixfix_expr& exp) {
		if (exp.OP == "call") {
			auto func_exp = exp.Operands[0];
			yvec<type_symbol*> parameters;
			if (exp.Operands.size() == 2) {
				auto args = exp.Operands[1];
				if (auto arg_list = dynamic_cast<list_expr*>(args)) {
					for (auto arg : arg_list->List) {
						parameters.push_back((*this)(*arg));
					}
				}
				else {
					parameters.push_back((*this)(*args));
				}
			}
			// Pseudo-function
			auto pseudo_func = new func_type_symbol(parameters, nullptr);
			func_exp->Hint = pseudo_func;
			auto func_t = (*this)(*func_exp);
			if (func_t) {
				if (auto func_ts = dynamic_cast<func_type_symbol*>(func_t)) {
					exp.EvalType = func_ts->ReturnType;
					return exp.EvalType;
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

	type_symbol* expr_checker::check(func_proto& exp) {
		yvec<type_symbol*> args;
		for (auto arg : exp.Parameters) {
			args.push_back(m_Checker.check_type(arg->Type));
		}
		auto rett = m_Checker.check_type(exp.ReturnType);
		exp.EvalType = new func_type_symbol(args, rett);
		return exp.EvalType;
	}

	type_symbol* expr_checker::check(func_expr& exp) {
		m_Table.push(new scope());
		auto protot = (*this)(*exp.Prototype);
		yset<ystr> arg_names;
		for (auto arg : exp.Prototype->Parameters) {
			if (arg->Name.some()) {
				auto arg_n = arg->Name.get().Identifier();
				if (arg_names.find(arg_n) != arg_names.end()) {
					throw std::exception("Parameter already defined!");
				}
				arg_names.insert(arg_n);
			}
			else {
				// TODO: WARN
			}
		}
		m_Checker.check_expr(exp.Body);
		m_Table.pop();
		
		exp.EvalType = protot;
		return exp.EvalType;
	}

	type_symbol* expr_checker::check(body_expr& exp) {
		m_Checker.check(exp.Statements);

		// TODO
		return nullptr;
	}

	type_symbol* expr_checker::check(param_expr& exp) {
		// TODO
		return nullptr;
	}

	type_symbol* expr_checker::check(let_expr& exp) {
		type_symbol* hintt = nullptr;
		type_symbol* rett = nullptr;
		if (exp.Type) hintt = m_Checker.check_type(exp.Type);
		if (exp.Value) {
			exp.Value->Hint = hintt;
			rett = (*this)(*exp.Value);
			if (!rett) {
				throw std::exception("Use of undeduced type!");
			}
			if (hintt) {
				if (!rett->same(hintt)) {
					throw std::exception("Wrong type for let!");
				}
			}
		}
		auto defines = let_pat::define(exp.Left, exp.Value);
		for (auto def : defines) {
			ystr const& ID = def.first;
			expr* VALUE = def.second;

			m_Table.decl(new var_symbol(ID, VALUE->EvalType));
			exp.Matched.push_back(def);
		}
		exp.EvalType = symbol_table::UNIT;
		return exp.EvalType;
	}
}