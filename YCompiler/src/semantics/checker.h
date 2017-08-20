#pragma once

#include "../common.h"

struct AST_stmt;
struct type_cons;

namespace checker {
	struct semantic_err;

	using result_t = yopt<semantic_err>;

	extern symbol_table SymTab;

	extern type_cons* UNIT;
	extern type_cons* I32;
	extern type_cons* F32;

	void init();
	result_t check_program(yvec<AST_stmt*>& prg);
}
