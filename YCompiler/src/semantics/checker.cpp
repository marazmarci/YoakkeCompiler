#include <cassert>
#include "checker.h"
#include "symbol_table.h"
#include "../syntax/ast_stmt.h"
#include "../syntax/ast_expr.h"
#include "../syntax/ast_ty.h"
#include "../syntax/ast_pat.h"

#define UNIMPLEMENTED assert(false && "Unimplemented!")
// TODO: Move this to the common header maybe?
#define UNREACHABLE assert(false && "Unreachable!");

namespace checker {
	namespace {
		symbol_table SymTab;
	}

	void check(AST_stmt* stmt) {
		switch (stmt->Ty) {
			case AST_stmt_t::Decl: {
				AST_decl_stmt* st = (AST_decl_stmt*)stmt;
				UNIMPLEMENTED;
			}

			case AST_stmt_t::Expr: {
				AST_expr_stmt* st = (AST_expr_stmt*)stmt;
				UNIMPLEMENTED;
			}
		}
		UNREACHABLE;
	}

	type* check(AST_expr* expr) {
		switch (expr->Ty) {
		case AST_expr_t::Bin: {
			UNIMPLEMENTED;
		}
		}
		UNREACHABLE;
	}

	type* check(AST_ty* ty) {
		UNREACHABLE;
	}

	void check(AST_pat* pat) {
		UNREACHABLE;
	}
}
