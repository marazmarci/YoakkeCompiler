#include "type_checker.h"
#include "symbol_table.h"
#include "type_symbol.h"

namespace yk {
	type_checker::type_checker(semantic_checker& ch, symbol_table& tab)
		: m_Checker(ch), m_Table(tab) {
		META_Visitables(ty_expr,
			ident_ty_expr, bin_ty_expr, list_ty_expr);
	}

	type_symbol* type_checker::check(ty_expr& td) {
		throw std::exception("Type check not covered!");
	}

	type_symbol* type_checker::check(ident_ty_expr& td) {
		auto sym_set = m_Table.ref(td.identifier);
		auto t_set = symbol_table::filter<type_symbol>(sym_set);
		if (t_set.size() == 0) {
			throw std::exception("Unkown type!");
		}
		else if (t_set.size() == 1) {
			td.SymbolForm = t_set[0];
			return td.SymbolForm;
		}
		else {
			throw std::exception("Type is ambigous!");
		}
	}

	type_symbol* type_checker::check(bin_ty_expr& td) {
		if (td.OP.identifier() == "->") {
			// TODO: maybe switch to tuple args instead of list?
			yvec<type_symbol*> arg_syms;
			auto args = (*this)(*td.LHS);
			auto rett = (*this)(*td.RHS);
			if (auto arglist = dynamic_cast<tuple_type_symbol*>(args)) {
				arg_syms = arglist->Types;
			}
			else {
				arg_syms.push_back(args);
			}
			td.SymbolForm = new func_type_symbol(arg_syms, rett);
			return td.SymbolForm;
		}
		else {
			throw std::exception("Type desc unhandled binop!");
		}
	}

	type_symbol* type_checker::check(list_ty_expr& td) {
		yvec<type_symbol*> types;
		for (auto t : td.List) {
			types.push_back((*this)(*t));
		}
		td.SymbolForm = new tuple_type_symbol(types);
		return td.SymbolForm;
	}
}