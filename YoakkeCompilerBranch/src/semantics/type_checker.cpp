#include "type_checker.h"

namespace yk {
	type_checker::type_checker(semantic_checker& ch, symbol_table& tab)
		: m_Checker(ch), m_Table(tab) {
	}

	void type_checker::dispatch(ident_type_desc* stmt) {

	}

	void type_checker::dispatch(bin_type_desc* stmt) {

	}

	void type_checker::dispatch(enclose_type_desc* stmt) {

	}
}