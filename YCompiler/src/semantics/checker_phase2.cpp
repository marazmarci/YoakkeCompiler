#include <iostream>
#include "checker.h"
#include "checker_phase2.h"
#include "symbol.h"
#include "scope.h"
#include "type.h"
#include "unifier.h"

namespace checker_phase2 {
	symbol_table& SymTab = checker::SymTab;

	using elem_t = ytup<ystr, type*>;

	type* generate_let_pattern(AST_pat* pat, yvec<elem_t>& buff) {
		switch (pat->Ty) {
		case AST_pat_t::List: {
			AST_list_pat* pt = (AST_list_pat*)pat;
			type_cons* tlist = new type_cons(type_prefixes::Tuple);
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

	void init(symbol_table& symt) {
		SymTab = symt;
	}

	result_t check(AST_stmt* st) {
		switch (st->Ty) {
		case AST_stmt_t::Decl: {
			auto stmt = (AST_decl_stmt*)st;
			auto res = check(stmt->Expression);
			if (res.is_err()) {
				return res.get_err();
			}
			type* ty = res.get_ok();
			if (auto err = unifier::unify(ty, stmt->Symbol->Type)) {
				std::cout << *err << std::endl;
				assert(false && "Unification error!");
			}
			// TODO: overloading
			return {};
		}

		case AST_stmt_t::TyDecl: {
			auto stmt = (AST_ty_decl_stmt*)st;
			auto res = check(stmt->Type);
			if (res.is_err()) {
				return res.get_err();
			}
			type* ty = res.get_ok();
			if (auto err = unifier::unify(ty, stmt->TypeSym)) {
				assert(false && "Unification error!");
			}
			return {};
		}

		case AST_stmt_t::Expr: {
			auto stmt = (AST_expr_stmt*)st;
			auto res = check(stmt->Expression);
			if (res.is_err()) {
				return res.get_err();
			}
			type* ty = res.get_ok();
			if (!unifier::same(ty, UNIT)) {
				// TODO: real warning
				std::cout << "Warn: ignored return value" << std::endl;
			}
			return {};
		}

		case AST_stmt_t::DbgWriteTy: {
			auto stmt = (AST_dbg_write_ty_stmt*)st;
			auto res = check(stmt->Expression);
			if (res.is_err()) {
				std::cout << "error" << std::endl;
				assert(false);
			}
			type* ty = res.get_ok();
			std::cout << "Type: " << unifier::to_str(ty) << std::endl;
			return {};
		}

		default:
			UNIMPLEMENTED;
		}

		UNREACHABLE;
	}

	res2_t check(AST_expr* ex) {
		switch (ex->Ty) {
		case AST_expr_t::Fn: {
			auto expr = (AST_fn_expr*)ex;
			SymTab.push_scope(expr->Scope);
			type_cons* params_t = new type_cons(type_prefixes::Params);
			for (auto& par : expr->Params) {
				auto& name = std::get<0>(par);
				auto& ty = std::get<1>(par);

				auto res = check(ty);
				if (res.is_err()) {
					return res.get_err();
				}
				auto& tty = res.get_ok();
				params_t->add(tty);

				if (name) {
					SymTab.decl(new var_symbol(name->Value, tty));
				}
				else {
					// TODO
					std::cout << "todo: Warning unnamed param" << std::endl;
				}
			}
			type* body_t = nullptr;
			{
				auto res = check(expr->Body);
				if (res.is_err()) {
					return res.get_err();
				}
				body_t = res.get_ok();
			}
			type* rett = UNIT;
			if (expr->Return) {
				auto res = check(*expr->Return);
				if (res.is_err()) {
					return res.get_err();
				}
				rett = res.get_ok();
			}
			if (auto err = unifier::unify(rett, body_t)) {
				// TODO
				std::cout << *err << std::endl;
				assert(false);
			}
			expr->Scope->ReturnType = rett;
			SymTab.pop_scope();

			return type_cons::fn(params_t, rett);
		}

		case AST_expr_t::Block: {
			auto expr = (AST_block_expr*)ex;
			SymTab.push_scope(expr->Scope);
			for (auto& st : expr->Statements) {
				if (auto err = check(st)) {
					return result_t(*err);
				}
			}
			type* rett = UNIT;
			if (expr->Value) {
				auto& val = *expr->Value;
				auto res = check(val);
				if (res.is_err()) {
					return res.get_err();
				}
				rett = res.get_ok();
			}

			SymTab.pop_scope();
			return rett;
		}

		case AST_expr_t::Ident: {
			auto expr = (AST_ident_expr*)ex;
			if (auto res = SymTab.ref_sym(expr->Value)) {
				auto& rres = *res;
				if (rres->Ty == symbol_t::Constant) {
					auto sym = (const_symbol*)rres;
					return sym->Type;
				}
				if (rres->Ty == symbol_t::Variable) {
					auto sym = (var_symbol*)rres;
					return sym->Type;
				}
				UNIMPLEMENTED;
			}
			else {
				std::cout << "Undefined symbol: '" << expr->Value << "'!" << std::endl;
				assert(false);
			}
		}

		case AST_expr_t::List: {
			auto expr = (AST_list_expr*)ex;
			if (expr->Elements.size() == 0) {
				return UNIT;
			}
			type_cons* ls = new type_cons(type_prefixes::Tuple, {});
			for (auto& td : expr->Elements) {
				auto res = check(td);
				if (res.is_err()) {
					return res.get_err();
				}
				auto& ty = res.get_ok();
				ls->add(ty);
			}
			return ls;
		}

		case AST_expr_t::IntLit: {
			// TODO: Better literals
			return I32;
		}

		case AST_expr_t::RealLit: {
			// TODO: Better literals
			return F32;
		}

		case AST_expr_t::Let: {
			auto expr = (AST_let_expr*)ex;

			auto res = generate_let_pattern(expr->Pattern);
			auto& entries = std::get<0>(res);
			auto& pat_ty = std::get<1>(res);

			if (expr->Type) {
				auto res = check(*expr->Type);
				if (res.is_err()) {
					return res.get_err();
				}
				auto& ty = res.get_ok();
				if (auto err = unifier::unify(pat_ty, ty)) {
					// TODO
					std::cout << *err << std::endl;
					assert(false);
				}
			}
			if (expr->Value) {
				auto res = check(*expr->Value);
				if (res.is_err()) {
					return res.get_err();
				}
				auto& ty = res.get_ok();
				if (auto err = unifier::unify(pat_ty, ty)) {
					// TODO
					std::cout << *err << std::endl;
					assert(false);
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

		default:
			UNIMPLEMENTED;
		}

		UNREACHABLE;
	}

	res2_t check(AST_ty* t) {
		switch (t->Ty) {
		case AST_ty_t::Pre: {
			UNIMPLEMENTED;
		}

		case AST_ty_t::Bin: {
			auto ty = (AST_bin_ty*)t;
			auto res = check(ty->Left);
			if (res.is_err()) {
				return res.get_err();
			}
			auto res2 = check(ty->Right);
			if (res2.is_err()) {
				return res2.get_err();
			}
			if (ty->Oper.Type == token_t::Arrow) {
				type* params = res.get_ok();
				if (params->Ty == type_t::Constructor) {
					auto params_t = (type_cons*)params;

					return type_cons::fn(
						new type_cons(type_prefixes::Params, params_t->Params), 
						res2.get_ok());
				}
				else {
					assert(false);
				}
			}
			else {
				std::cout << "Unknown type operator" << std::endl;
				assert(false);
			}
		}

		case AST_ty_t::Post: {
			UNIMPLEMENTED;
		}

		case AST_ty_t::List: {
			auto ty = (AST_list_ty*)t;
			type_cons* tup = new type_cons(type_prefixes::Tuple);
			if (ty->Elements.size() == 0) {
				return UNIT;
			}
			for (auto& elem : ty->Elements) {
				auto res = check(elem);
				if (res.is_err()) {
					return res.get_err();
				}
				tup->add(res.get_ok());
			}
			return tup;
		}

		case AST_ty_t::Ident: {
			auto ty = (AST_ident_ty*)t;

			if (auto tsym = SymTab.ref_type(ty->Value)) {
				return *tsym;
			}
			else {
				std::cout << "Undefined type: '" << ty->Value << "'" << std::endl;
				assert(false);
			}
		}

		default:
			UNIMPLEMENTED;
		}

		UNREACHABLE;
	}
}