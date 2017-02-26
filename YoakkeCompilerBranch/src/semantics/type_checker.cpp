#include "type_checker.h"
#include "symbol_table.h"
#include "type_symbol.h"
#include "algorithms.h"

namespace yk {
	type_checker::type_checker(semantic_checker& ch, symbol_table& tab)
		: m_Checker(ch), m_Table(tab) {
	}

	type_symbol* type_checker::dispatch(ident_type_desc* td) {
		auto sym_set = m_Table.ref(td->Identifier);
		auto t_set = symbol_table::filter<type_symbol>(sym_set);
		if (t_set.size() == 0) {
			throw std::exception("Unkown type!");
		}
		else if (t_set.size() == 1) {
			return t_set[0];
		}
		else {
			throw std::exception("Type is ambigous!");
		}
	}

	type_symbol* type_checker::dispatch(bin_type_desc* td) {
		if (td->OP.identifier() == ",") {
			auto flat = alg::flatten<type_desc, bin_type_desc>(td, ",");
			yvec<type_symbol*> types;
			for (auto t : flat) {
				types.push_back(dispatch_gen(t));
			}
			return new tuple_type_symbol(types);
		}
		else if (td->OP.identifier() == "->") {
			// TODO: maybe switch to tuple args instead of list?
			yvec<type_symbol*> arg_syms;
			auto args = dispatch_gen(td->LHS);
			auto rett = dispatch_gen(td->RHS);
			if (auto arglist = dynamic_cast<tuple_type_symbol*>(args)) {
				arg_syms = arglist->Types;
			}
			else {
				arg_syms.push_back(args);
			}
			return new func_type_symbol(arg_syms, rett);
		}
		else {
			throw std::exception("Type desc unhandled binop!");
		}
	}

	type_symbol* type_checker::dispatch(enclose_type_desc* td) {
		return dispatch_gen(td->Sub);
	}
}