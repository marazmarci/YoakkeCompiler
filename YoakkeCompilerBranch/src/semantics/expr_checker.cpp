#include "expr_checker.h"
#include "symbol_table.h"
#include "typed_symbol.h"
#include "algorithms.h"
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
			typed_set = m_Table.filter_typed(typed_set, exp->Hint);
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
			typed_set = symbol_table::filter_typed(typed_set, rhs_type);
			if (typed_set.size()) {
				throw std::exception("Ambigious constant binding is not allowed!");
			}
			else {
				m_Table.decl(new const_bind_symbol(ident->identifier, rhs_type, exp->RHS));
				return symbol_table::UNIT;
			}
		}
		else if (exp->OP.identifier() == ",") {
			auto flat = alg::flatten<expr, bin_expr>(exp, ",");
			yvec<type_symbol*> types;
			for (auto el : flat) {
				types.push_back(dispatch_gen(el));
			}
			return new tuple_type_symbol(types);
		}
		return nullptr;
	}

	type_symbol* expr_checker::dispatch(preury_expr* exp) {
		return nullptr;
	}

	type_symbol* expr_checker::dispatch(postury_expr* exp) {
		return nullptr;
	}

	type_symbol* expr_checker::dispatch(enclose_expr* exp) {
		exp->Sub->Hint = exp->Hint;
		return dispatch_gen(exp->Sub);
	}

	type_symbol* expr_checker::dispatch(mixfix_expr* exp) {
		return nullptr;
	}

	type_symbol* expr_checker::dispatch(func_proto* exp) {
		return nullptr;
	}

	type_symbol* expr_checker::dispatch(func_expr* exp) {
		// TODO
		m_Checker.check_expr(exp->Body);
		
		return nullptr;
	}

	type_symbol* expr_checker::dispatch(body_expr* exp) {
		for (auto st : exp->Statements) {
			m_Checker.check_stmt(st);
		}

		// TODO
		return nullptr;
	}

	type_symbol* expr_checker::dispatch(param_expr* exp) {
		return nullptr;
	}

	type_symbol* expr_checker::dispatch(let_expr* exp) {
		type_symbol* hintt = nullptr;
		type_symbol* rett = nullptr;
		if (exp->Type) hintt = m_Checker.check_type(exp->Type);
		if (exp->Value) {
			exp->Hint = hintt;
			rett = dispatch_gen(exp->Value);
		}
		let_pat::define(m_Table, exp->Left, rett);
		return symbol_table::UNIT;
	}
}