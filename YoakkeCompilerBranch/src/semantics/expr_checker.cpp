#include "expr_checker.h"
#include "symbol_table.h"
#include "typed_symbol.h"

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

	type_symbol* expr_checker::dispatch(bin_expr* exp) {
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
		return nullptr;
	}

	type_symbol* expr_checker::dispatch(body_expr* exp) {
		return nullptr;
	}

	type_symbol* expr_checker::dispatch(param_expr* exp) {
		return nullptr;
	}

	type_symbol* expr_checker::dispatch(let_expr* exp) {
		return nullptr;
	}
}