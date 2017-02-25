#include "type_checker.h"
#include "symbol_table.h"
#include "type_symbol.h"

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
		return nullptr;
	}

	type_symbol* type_checker::dispatch(enclose_type_desc* td) {
		return nullptr;
	}
}