#include <iostream>
#include "checker.h"
#include "checker_phase1.h"
#include "symbol.h"
#include "symbol_table.h"
#include "type.h"
#include "unifier.h"
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
		case AST_stmt_t::Decl: {
			auto stmt = (AST_decl_stmt*)st;
			ystr const& name = stmt->Name.Value;
			if (auto err = check(stmt->Expression)) {
				return *err;
			}
			// TODO: check for shadowing
			if (auto ref = SymTab.local_ref_sym(name)) {
				auto& reff = *ref;
				if (stmt->DeclType == AST_decl_t::Const) {
					// TODO: is the reff is function, return another error
					// saying you cannot define a function and const
					// with the same name in the same scope
					return already_def_err(
						"constant", name,
						reff->DefPos, stmt->Name.Pos);
				}
				else {
					if (reff->Ty == symbol_t::Variable) {
						assert(false);
					}
					if (reff->Ty == symbol_t::Constant) {
						auto other_sym = (const_symbol*)reff;
						auto fn_ty = type_cons::generic_fn();
						if (auto err = unifier::unify(fn_ty, other_sym->Type)) {
							return type_unify_err(*err, stmt->Pos);
						}

						auto tyc = new typeclass_symbol(name);
						type* ty = type_cons::generic_fn();
						tyc->add(fn_ty);	// Already existing
						tyc->add(ty);		// Current
						stmt->TypeSym = ty;

						if (!SymTab.remove_symbol(name)) {
							assert(false);
						}
						SymTab.decl(tyc);
					}
					if (reff->Ty == symbol_t::Typeclass) {
						auto tyclass = (typeclass_symbol*)reff;
						
						type* ty = type_cons::generic_fn();
						tyclass->add(ty);
						stmt->TypeSym = ty;
					}
				}
			}
			else {
				type* ty = type_cons::generic_fn();
				const_symbol* sym = new const_symbol(name, ty);
				SymTab.decl(sym);
				stmt->TypeSym = ty;
			}
		}
		break;

		case AST_stmt_t::TyDecl: {
			auto stmt = (AST_ty_decl_stmt*)st;
			ystr const& name = stmt->Name.Value;
			if (auto ref = SymTab.local_ref_type(name)) {
				auto reff = *ref;
				return already_def_err(
					"type", name,
					reff->DefPos, stmt->Name.Pos);
			}
			else {
				if (auto ref = SymTab.upper_ref_type(name)) {
					// TODO: Real warn
					auto reff = *ref;
					std::cout << "Warning: shadowing type: '" << name << "'!" << std::endl;
				}
				type_var* ty = new type_var();
				ty->DefPos = stmt->Name.Pos;
				SymTab.decl(name, ty);
				stmt->TypeSym = ty;
			}
		}
		break;

		case AST_stmt_t::Expr: {
			auto stmt = (AST_expr_stmt*)st;
			return check(stmt->Expression);
		}
		break;

		case AST_stmt_t::DbgWriteTy: {
		}
		break;

		default:
			UNIMPLEMENTED;
		}

		return {};
	}

	result_t check(AST_expr* ex) {
		switch (ex->Ty) {
		case AST_expr_t::Pre: {
			auto expr = (AST_pre_expr*)ex;
			if (auto err = check(expr->Subexpr)) {
				return *err;
			}
		}
		break;

		case AST_expr_t::Bin: {
			auto expr = (AST_bin_expr*)ex;
			if (auto err = check(expr->Left)) {
				return *err;
			}
			if (auto err = check(expr->Right)) {
				return *err;
			}
		}
		break;

		case AST_expr_t::Post: {
			auto expr = (AST_post_expr*)ex;
			if (auto err = check(expr->Subexpr)) {
				return *err;
			}
		}
		break;

		case AST_expr_t::Block: {
			auto expr = (AST_block_expr*)ex;
			expr->Scope = SymTab.push_scope(!expr->AsStatement);
			for (auto& st : expr->Statements) {
				if (auto err = check(st)) {
					return *err;
				}
			}
			if (expr->Value) {
				auto val = *expr->Value;
				if (auto err = check(val)) {
					return *err;
				}
			}
			SymTab.pop_scope();
		}
		break;

		case AST_expr_t::Call: {
			auto expr = (AST_call_expr*)ex;
			if (auto err = check(expr->Func)) {
				return *err;
			}
			for (auto& param : expr->Params) {
				if (auto err = check(param)) {
					return *err;
				}
			}
		}
		break;

		case AST_expr_t::Fn: {
			auto expr = (AST_fn_expr*)ex;
			expr->Scope = SymTab.push_scope(true);
			if (auto err = check(expr->Body)) {
				return *err;
			}
			SymTab.pop_scope();
		}
		break;

		case AST_expr_t::If: {
			auto expr = (AST_if_expr*)ex;
			if (auto err = check(expr->Condition)) {
				return *err;
			}
			if (auto err = check(expr->Then)) {
				return *err;
			}
			if (expr->Else) {
				auto& el = *expr->Else;
				if (auto err = check(el)) {
					return *err;
				}
			}
		}
		break;

		case AST_expr_t::Let: {
			auto expr = (AST_let_expr*)ex;
			if (expr->Value) {
				auto& val = *expr->Value;
				if (auto err = check(val)) {
					return *err;
				}
			}
		}
		break;

		case AST_expr_t::List: {
			auto expr = (AST_list_expr*)ex;
			for (auto& el : expr->Elements) {
				if (auto err = check(el)) {
					return *err;
				}
			}
		}
		break;

		case AST_expr_t::RealLit:
		case AST_expr_t::IntLit:
		case AST_expr_t::Ident: {
		}
		break;

		default:
			UNIMPLEMENTED;
		}

		return {};
	}
}
