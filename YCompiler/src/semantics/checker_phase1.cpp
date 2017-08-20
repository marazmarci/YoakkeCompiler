#include "checker.h"
#include "checker_phase1.h"
#include "symbol_table.h"
#include "../syntax/ast_stmt.h"
#include "../syntax/ast_expr.h"
#include "../syntax/ast_ty.h"
#include "../syntax/ast_pat.h"

namespace checker_phase1 {
	symbol_table& SymTab = checker::SymTab;

	void init(symbol_table& symt) {
		SymTab = symt;
	}

	void check(AST_stmt* st) {
		switch (st->Ty) {
		case AST_stmt_t::TyDecl: {
			auto stmt = (AST_ty_decl_stmt*)st;
			SymTab.decl();
		}
		break;
		}
	}
}
