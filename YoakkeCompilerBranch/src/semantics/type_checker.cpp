#include "type_checker.h"

namespace yk {
	type_checker::type_checker(semantic_checker* ch)
		: m_Checker(ch) {
	}

	void type_checker::dispatch(ident_type_desc* stmt) {

	}

	void type_checker::dispatch(bin_type_desc* stmt) {

	}

	void type_checker::dispatch(enclose_type_desc* stmt) {

	}
}