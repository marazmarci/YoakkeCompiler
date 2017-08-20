#include "checker.h"
#include "checker_phase1.h"
#include "symbol_table.h"
#include "type.h"
#include "../syntax/ast_stmt.h"
#include "../syntax/ast_expr.h"
#include "../syntax/ast_ty.h"
#include "../syntax/ast_pat.h"

namespace checker_phase1 {
	using namespace checker;

	symbol_table& SymTab = checker::SymTab;

	void init(symbol_table& symt) {
		SymTab = symt;
	}

	result_t check(AST_stmt* st) {
		switch (st->Ty) {
		case AST_stmt_t::TyDecl: {
			auto stmt = (AST_ty_decl_stmt*)st;
			if (auto ref = SymTab.ref_type(stmt->Name.Value)) {
				auto reff = *ref;
				return already_def_err(
					"type", stmt->Name.Value, 
					reff->DefPos, stmt->Name.Pos);
			}
			else {
				type_var* ty = new type_var();
				ty->DefPos = stmt->Name.Pos;
				SymTab.decl(stmt->Name.Value, ty);
				stmt->TypeSym = ty;
			}
		}
		break;
		}

		return {};
	}
}
