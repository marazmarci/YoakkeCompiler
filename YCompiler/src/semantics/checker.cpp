#include <cassert>
#include <iostream>
#include "type.h"
#include "symbol.h"
#include "checker.h"
#include "symbol_table.h"
#include "../syntax/ast_stmt.h"
#include "../syntax/ast_expr.h"
#include "../syntax/ast_ty.h"
#include "../syntax/ast_pat.h"

namespace checker {
	namespace {
		symbol_table SymTab;

		type_cons* UNIT = new type_cons("@tup", {});
		type_cons* I32	= new type_cons("i32", {});
		type_cons* F32	= new type_cons("f32", {});
	}

	void init() {
		SymTab.decl(UNIT);
		SymTab.decl(I32);
		SymTab.decl(F32);
	}

	void check(AST_stmt* stmt) {
		switch (stmt->Ty) {
			case AST_stmt_t::Decl: {
				AST_decl_stmt* st = (AST_decl_stmt*)stmt;
				type* expr_t = check(st->Expression);
				if (expr_t->Ty == type_t::Variable) {
					std::cout << "Type var in declaration ERROR!" << std::endl;
					return;
				}
				if (expr_t->Ty == type_t::Constructor) {
					type_cons* tc = (type_cons*)expr_t;
					
					if (tc->Name == type_prefixes::Function) {

					}
				}
				std::cout << "SANITY error in decl!" << std::endl;
				return;
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
