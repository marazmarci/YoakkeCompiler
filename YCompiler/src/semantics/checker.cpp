#include "checker.h"
#include "symbol_table.h"
#include "type.h"
#include "../syntax/ast_stmt.h"
#include "checker_phase1.h"
#include "checker_phase2.h"
#include "checker_phase3.h"

namespace checker {
	symbol_table SymTab;

	type_cons* UNIT;
	type_cons* I32;
	type_cons* F32;

	void init() {
		SymTab.decl(UNIT = new type_cons(type_prefixes::Tuple));
		SymTab.decl(I32 = new type_cons("i32"));
		SymTab.decl(F32 = new type_cons("f32"));
	}

	result_t check_program(yvec<AST_stmt*>& prg) {
		init();

		checker_phase1::init(SymTab);
		checker_phase2::init(SymTab);
		checker_phase3::init(SymTab);
	}
}

