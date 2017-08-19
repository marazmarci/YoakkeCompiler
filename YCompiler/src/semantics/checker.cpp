#include <cassert>
#include <iostream>
#include "type.h"
#include "symbol.h"
#include "checker.h"
#include "symbol_table.h"
#include "unifier.h"
#include "scope.h"
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

		using elem_t = ytup<ystr, type*>;

		type* generate_let_pattern(AST_pat* pat, yvec<elem_t>& buff) {
			switch (pat->Ty) {
			case AST_pat_t::List: {
				AST_list_pat* pt = (AST_list_pat*)pat;
				type_cons* tlist = new type_cons(type_prefixes::Tuple, {});
				for (auto& elem : pt->Elements) {
					tlist->add(generate_let_pattern(elem, buff));
				}
				return tlist;
			}

			case AST_pat_t::Ident: {
				AST_ident_pat* pt = (AST_ident_pat*)pat;
				type_var* tv = new type_var();
				buff.push_back({ pt->Value, tv });
				return tv;
			}

			case AST_pat_t::Bin: {
				UNIMPLEMENTED;
			}

			case AST_pat_t::Pre: {
				UNIMPLEMENTED;
			}

			case AST_pat_t::Post: {
				UNIMPLEMENTED;
			}
			}
			UNREACHABLE;
		}

		ytup<yvec<elem_t>, type*> generate_let_pattern(AST_pat* pat) {
			yvec<elem_t> buff;
			type* t = generate_let_pattern(pat, buff);
			return { buff, t };
		}
	}

	void init() {
		SymTab.decl(UNIT);
		SymTab.decl(I32);
		SymTab.decl(F32);
	}

	void pre_check(AST_stmt* stmt) {
		switch (stmt->Ty) {
		case AST_stmt_t::Decl: {
			AST_decl_stmt* st = (AST_decl_stmt*)stmt;
			auto expr_tt = pre_check(st->Expression);
			if (!expr_tt) {
				std::cout << "SANITY IN predecl!" << std::endl;
				return;
			}
			auto expr_t = *expr_tt;
			if (expr_t->Ty == type_t::Variable) {
				std::cout << "Type var in pre-declaration ERROR!" << std::endl;
				return;
			}
			if (expr_t->Ty == type_t::Constructor) {
				type_cons* tc = (type_cons*)expr_t;
				ystr const& name = st->Name.Value;

				if (/*auto already_def = */SymTab.ref_sym(name)) {
					// TODO
					if (/*auto already_def2 = */SymTab.local_ref_sym(name)) {
						std::cout
							<< "TODO: error '" << name << "' already defined!"
							<< std::endl;
						if (tc->Name == type_prefixes::Function) {
							std::cout << "(func overload)" << std::endl;
						}
					}
					else {
						std::cout << "Info: Shadowing '" << name << "'!" << std::endl;
						SymTab.decl(new const_symbol(name, expr_t));
					}
				}
				else {
					SymTab.decl(new const_symbol(name, expr_t));
				}
				return;
			}
			UNREACHABLE;
			return;
		}

		case AST_stmt_t::Expr: {
			AST_expr_stmt* st = (AST_expr_stmt*)stmt;
			pre_check(st->Expression);
			return;
		}

		default:
			return;
		}
		UNREACHABLE;
	}

	yopt<type*> pre_check(AST_expr* expr) {
		switch (expr->Ty) {
		case AST_expr_t::Block: {
			AST_block_expr* ex = (AST_block_expr*)expr;
			ex->Scope = SymTab.push_scope(!ex->AsStatement);
			for (auto& st : ex->Statements) {
				pre_check(st);
			}
			if (ex->Value) {
				pre_check(*ex->Value);
			}
			SymTab.pop_scope();
			return {};
		}

		case AST_expr_t::Fn: {
			AST_fn_expr* ex = (AST_fn_expr*)expr;
			ex->Scope = SymTab.push_scope(true);
			type_cons* params_t = new type_cons(type_prefixes::Params, {});
			for (auto& par : ex->Params) {
				auto& name = std::get<0>(par);
				auto& ty = std::get<1>(par);

				type* tty = check(ty);
				params_t->add(tty);

				if (name) {
					SymTab.decl(new var_symbol(name->Value, tty));
				}
				else {
					// TODO
					std::cout << "todo: Warning unnamed param" << std::endl;
				}
			}
			type* rett = UNIT;
			if (ex->Return) {
				rett = check(*ex->Return);
			}
			SymTab.pop_scope();

			return new type_cons(
				type_prefixes::Function, { params_t, rett }
			);
		}

		default:
			return {};
		}
		UNREACHABLE;
	}

	void check(AST_stmt* stmt) {
		switch (stmt->Ty) {
			case AST_stmt_t::Decl: {
				AST_decl_stmt* st = (AST_decl_stmt*)stmt;
				check(st->Expression);
				return;
			}

			case AST_stmt_t::Expr: {
				AST_expr_stmt* st = (AST_expr_stmt*)stmt;
				type* expr_t = check(st->Expression);
				if (!unifier::same(expr_t, UNIT)) {
					std::cout << "Info: ignoring return value!" << std::endl;
				}
				return;
			}

			case AST_stmt_t::DbgWriteTy: {
				auto st = (AST_dbg_write_ty_stmt*)stmt;
				type* t = check(st->Expression);
				std::cout << "dbg type: " << unifier::to_str(t) << std::endl;
				return;
			}
		}
		UNREACHABLE;
	}

	type* check(AST_expr* expr) {
		switch (expr->Ty) {
		case AST_expr_t::Block: {
			AST_block_expr* ex = (AST_block_expr*)expr;
			SymTab.set_scope(ex->Scope);
			for (auto& st : ex->Statements) {
				check(st);
			}
			type* ret_ty = UNIT;
			if (ex->Value) {
				AST_expr* ret = *ex->Value;
				ret_ty = check(ret);
				auto ret_sc = SymTab.nearest_ret_dest();
				if (!ret_sc) {
					// TODO
					std::cout << "todo: No return destination!" << std::endl;
					return nullptr;
				}
				if (auto err = unifier::unify(ret_ty, (*ret_sc)->ReturnType)) {
					std::cout << *err << std::endl;
					return nullptr;
				}
			}
			SymTab.pop_scope();
			return ret_ty;
		}

		case AST_expr_t::Call: {
			UNIMPLEMENTED;
		}

		case AST_expr_t::Fn: {
			AST_fn_expr* ex = (AST_fn_expr*)expr;
			SymTab.set_scope(ex->Scope);
			type* rett = UNIT;
			if (ex->Return) {
				rett = check(*ex->Return);
			}
			type* bodyt = check(ex->Body);
			if (auto err = unifier::unify(rett, bodyt)) {
				std::cout << *err << std::endl;
				// TODO
				return nullptr;
			}
			ex->Scope->ReturnType = rett;
			SymTab.pop_scope();

			return ex->DeclType;
		}

		case AST_expr_t::If: {
			UNIMPLEMENTED;
		}

		case AST_expr_t::Let: {
			AST_let_expr* ex = (AST_let_expr*)expr;
			auto res = generate_let_pattern(ex->Pattern);
			auto& entries = std::get<0>(res);
			auto& pat_ty = std::get<1>(res);

			if (ex->Type) {
				type* ty = check(*ex->Type);
				if (auto err = unifier::unify(pat_ty, ty)) {
					std::cout << *err << std::endl;
					return nullptr;
				}
			}
			if (ex->Value) {
				type* ty = check(*ex->Value);
				if (auto err = unifier::unify(pat_ty, ty)) {
					std::cout << *err << std::endl;
					return nullptr;
				}
			}
			for (auto& entry : entries) {
				auto& name = std::get<0>(entry);
				auto& ty = std::get<1>(entry);
				if (SymTab.shadow_symbol(name)) {
					std::cout << "Warning: shadowing '" << name << "'" << std::endl;
				}
				SymTab.decl(new var_symbol(name, ty));
			}
			// Let always returns unit
			return UNIT;
		}

		case AST_expr_t::List: {
			AST_list_expr* ex = (AST_list_expr*)expr;
			if (ex->Elements.size() == 0) {
				return UNIT;
			}
			type_cons* ls = new type_cons(type_prefixes::Tuple, {});
			for (auto& td : ex->Elements) {
				ls->add(check(td));
			}
			return ls;
		}

		case AST_expr_t::Ident: {
			AST_ident_expr* ex = (AST_ident_expr*)expr;
			if (auto res = SymTab.ref_sym(ex->Value)) {
				symbol* sym = *res;
				if (sym->Ty == symbol_t::Constant) {
					const_symbol* cs = (const_symbol*)sym;
					return cs->Type;
				}
				else if (sym->Ty == symbol_t::Variable) {
					var_symbol* vs = (var_symbol*)sym;
					return vs->Type;
				}
				else {
					std::cout << "UNSUPPORTED!" << std::endl;
				}
			}
			else {
				std::cout << "Undefined symbol: " << ex->Value << std::endl;
			}
			return nullptr;
		}

		case AST_expr_t::IntLit: {
			// TODO: Better literals
			return I32;
		}

		case AST_expr_t::RealLit: {
			// TODO: Better literals
			return F32;
		}

		case AST_expr_t::Pre: {
			UNIMPLEMENTED;
		}
		
		case AST_expr_t::Bin: {
			UNIMPLEMENTED;
		}

		case AST_expr_t::Post: {
			UNIMPLEMENTED;
		}
		}
		UNREACHABLE;
	}

	type* check(AST_ty* ty) {
		switch (ty->Ty) {
		case AST_ty_t::Pre: {
			UNIMPLEMENTED;
		}

		case AST_ty_t::Bin: {
			UNIMPLEMENTED;
		}

		case AST_ty_t::Post: {
			UNIMPLEMENTED;
		}

		case AST_ty_t::List: {
			UNIMPLEMENTED;
		}

		case AST_ty_t::Ident: {
			AST_ident_ty* tt = (AST_ident_ty*)ty;

			if (auto tsym = SymTab.ref_type(tt->Value)) {
				return *tsym;
			}
			else {
				std::cout << "Undefined type: '" << tt->Value << "'" << std::endl;
				return nullptr;
			}
		}
		}
		UNREACHABLE;
	}

	void check(AST_pat* pat) {
		UNREACHABLE;
	}
}
