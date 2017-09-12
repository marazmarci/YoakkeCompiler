#include <iostream>
#include "checker.h"
#include "oper_desc.h"
#include "scope.h"
#include "symbol.h"
#include "symbol_table.h"
#include "type.h"
#include "unifier.h"
#include "../syntax/ast_stmt.h"
#include "../syntax/ast_expr.h"
#include "../syntax/ast_ty.h"
#include "../syntax/ast_pat.h"
#include "../io/fmt_code.h"
#include "../functions.h"


yopt<semantic_err> checker::phase3(AST_stmt* st) {
	switch (st->Ty) {
	case AST_stmt_t::ConstDecl: UNIMPLEMENTED;

	case AST_stmt_t::FnDecl: {
		auto stmt = (AST_fn_decl_stmt*)st;
		auto res = phase3(stmt->Expression);
		if (res.is_err()) {
			return res.get_err();
		}
		return {};
	}

	case AST_stmt_t::OpDecl: {
		auto stmt = (AST_op_decl_stmt*)st;
		auto res = phase3(stmt->Expression);
		if (res.is_err()) {
			return res.get_err();
		}
		return {};
	}

	case AST_stmt_t::TyDecl: {
		return {};
	}

	case AST_stmt_t::Expr: {
		auto stmt = (AST_expr_stmt*)st;
		auto res = phase3(stmt->Expression);
		if (res.is_err()) {
			return res.get_err();
		}
		//auto& ty = res.get_ok();
		//if (!unifier::same(ty, UNIT)) {
		//	print_pointed_msg(
		//		"Warning: ignoring return value",
		//		to_sem_pos(stmt->Pos)
		//	);
		//}
		return {};
	}

	case AST_stmt_t::DbgWriteTy: {
		auto stmt = (AST_dbg_write_ty_stmt*)st;
		auto res = phase3(stmt->Expression);
		if (res.is_err()) {
			return res.get_err();
		}
		auto& ty = res.get_ok();
		std::cout << "DBG TY: " << unifier::to_str(ty) << std::endl;
		return {};
	}

	default: UNIMPLEMENTED;
	}

	UNREACHABLE;
	return {};
}

yresult<type*, semantic_err> checker::phase3(AST_expr* ex) {
	switch (ex->Ty) {
	case AST_expr_t::Block: {
		auto expr = (AST_block_expr*)ex;
		SymTab.push_scope(expr->Scope);	// ----------------------------
		for (auto& st : expr->Statements) {
			if (auto err = phase3(st)) {
				return *err;
			}
		}
		type* ret_t = nullptr;
		if (expr->Value) {
			auto& expr_val = *expr->Value;
			auto res = phase3(expr_val);
			if (res.is_err()) {
				return res.get_err();
			}
			ret_t = res.get_ok();
			auto n_ret_scope = SymTab.nearest_ret_dest();
			assert(n_ret_scope);
			auto ret_scope = *n_ret_scope;
			if (ret_scope->ReturnType) {
				if (auto err = unifier::unify(ret_scope->ReturnType, ret_t)) {
					return semantic_err(semantics_ty_err(
						"Type %a is not compatible with type %b %f!",
						unifier::to_str(ret_scope->ReturnType),
						unifier::to_str(ret_t),
						ret_scope->ReturnPos,
						to_sem_pos(expr_val->Pos)));
				}
			}
			else {
				auto& val = *expr->Value;
				ret_scope->ReturnPos = to_sem_pos(val->Pos);
				ret_scope->ReturnType = ret_t;
			}
		}
		else {
			if (expr->Scope->ReturnType) {
				ret_t = expr->Scope->ReturnType;
			}
			else {
				ret_t = UNIT;
			}
		}
		SymTab.pop_scope();	// ----------------------------------------
		return ret_t;
	}

	case AST_expr_t::Call: {
		auto expr = (AST_call_expr*)ex;
		auto res = phase3(expr->Func);
		if (res.is_err()) {
			return res.get_err();
		}
		auto& left_ty = res.get_ok();
		if (auto err = unifier::unify(left_ty, type_cons::generic_fn())) {
			return semantic_err(semantics_pos_err(
				"Semantic error: Tried to call non-callable expression",
				to_sem_pos(expr->Func->Pos)
			));
		}
		yvec<type*> param_list;
		for (auto& param : expr->Params) {
			auto res = phase3(param);
			if (res.is_err()) {
				return res.get_err();
			}
			param_list.push_back(res.get_ok());
		}
		type* param_t = single_or(param_list);
		type* ret_t = new type_var();
		type* exp_func = type_cons::fn(param_t, ret_t);
		if (auto err = unifier::unify(left_ty, exp_func)) {
			type_cons* left_c = (type_cons*)left_ty;
			auto left_par_t = left_c->Params[0];
			if (left_par_t->Ty == type_t::Constructor) {
				type_cons* par_t = (type_cons*)left_par_t;
				if (par_t->Params.size() != param_list.size()) {
					return semantic_err(semantics_pos_err(
						"Semantic error: Wrong number of arguments provided for call",
						to_sem_pos(expr->Func->Pos)
					));
				}
			}
			return semantic_err(semantics_pos_err(
				"Semantic error: Wrong arguments provided for call",
				to_sem_pos(expr->Func->Pos)
			));
		}
		return ret_t;
	}

	case AST_expr_t::Fn: {
		auto expr = (AST_fn_expr*)ex;
		SymTab.push_scope(expr->Scope);
		auto res = phase3(expr->Body);
		if (res.is_err()) {
			return res.get_err();
		}
		auto& body_t = res.get_ok();
		SymTab.pop_scope();
		assert(expr->Scope->ReturnType);
		return expr->Symbol;
	}

	case AST_expr_t::If: {
		auto expr = (AST_if_expr*)ex;
		SymTab.push_scope(expr->Scope);
		auto res = phase3(expr->Condition);
		if (res.is_err()) {
			return res.get_err();
		}
		auto& cond_ty = res.get_ok();
		if (auto err = unifier::unify(cond_ty, BOOL)) {
			return semantic_err(semantics_pos_err(
				"Semantic error: if condition is not evaluated to a boolean",
				to_sem_pos(expr->Condition->Pos)
			));
		}
		auto res2 = phase3(expr->Then);
		if (res2.is_err()) {
			return res2.get_err();
		}
		if (expr->Else) {
			auto res3 = phase3(*expr->Else);
			if (res3.is_err()) {
				return res3.get_err();
			}
		}
		SymTab.pop_scope();
		if (expr->AsStatement) {
			return UNIT;
		}
		else {
			assert(expr->Else);
			if (!expr->Scope->ReturnType) {
				return UNIT;
			}
			return expr->Scope->ReturnType;
		}
	}

	case AST_expr_t::Let: {
		auto expr = (AST_let_expr*)ex;
		type* left_t;
		yvec<ytup<ystr, interval, type*>> entries;
		std::tie(entries, left_t) = generate_let_pattern(expr->Pattern);
		if (expr->Type) {
			auto& typ = *expr->Type;
			auto res = check_ty(typ);
			if (res.is_err()) {
				return res.get_err();
			}
			auto& ty_t = res.get_ok();
			if (auto err = unifier::unify(left_t, ty_t)) {
				return semantic_err(semantics_ty_err(
					"Let type is not matching the pattern %f!",
					"",
					"",
					to_sem_pos(expr->Pattern->Pos),
					to_sem_pos(typ->Pos)
				));
			}
		}
		if (expr->Value) {
			auto& typ = *expr->Value;
			auto res = phase3(typ);
			if (res.is_err()) {
				return res.get_err();
			}
			auto& ty_t = res.get_ok();
			if (auto err = unifier::unify(left_t, ty_t)) {
				return semantic_err(semantics_ty_err(
					"Let value is not matching the type %f!",
					"",
					"",
					to_sem_pos(expr->Pattern->Pos),
					to_sem_pos(typ->Pos)
				));
			}
		}
		for (auto& entry : entries) {
			auto& name = std::get<0>(entry);
			auto& pos = std::get<1>(entry);
			auto& ty = std::get<2>(entry);

			auto sem_p = to_sem_pos(pos);
			var_symbol* sym = new var_symbol(name, ty);
			sym->DefPos = sem_p;

			if (auto n_other = SymTab.shadow_symbol(name)) {
				auto& other = *n_other;
				print_def_msg("Warning: shadowing %k %n %f!",
					"symbol", name, other->DefPos, sem_p);
			}

			SymTab.decl(sym);
		}
		return UNIT;
	}

	case AST_expr_t::List: {
		auto expr = (AST_list_expr*)ex;
		yvec<type*> type_list;
		for (auto& elem : expr->Elements) {
			auto res = phase3(elem);
			if (res.is_err()) {
				return res.get_err();
			}
			type_list.push_back(res.get_ok());
		}
		return single_or(type_list);
	}

	case AST_expr_t::Ident: {
		auto expr = (AST_ident_expr*)ex;
		if (auto n_sym = SymTab.ref_sym(expr->Value)) {
			auto& sym = *n_sym;
			if (sym->Ty == symbol_t::Constant) {
				auto csym = (const_symbol*)sym;
				return csym->Type;
			}
			else if (sym->Ty == symbol_t::Variable) {
				auto vsym = (var_symbol*)sym;
				return vsym->Type;
			}
			else if (sym->Ty == symbol_t::Typeclass) {
				auto tsym = (typeclass_symbol*)sym;
				auto entry = unifier::add_class_constraint(
					Constraints, tsym, to_sem_pos(expr->Pos));
				return entry;
			}
			else {
				UNREACHABLE;
			}
		}
		else {
			return semantic_err(semantics_def_err(
				"Semantic error: Undefined %k %n %f!",
				"symbol",
				expr->Value,
				{}, to_sem_pos(expr->Pos)
			));
		}
	}

	case AST_expr_t::Pre:
	case AST_expr_t::Post: {
		// TODO
		UNIMPLEMENTED;
	}

						   // TODO: Common code with Ident
	case AST_expr_t::Bin: {
		auto expr = (AST_bin_expr*)ex;
		auto res1 = phase3(expr->Left);
		if (res1.is_err()) {
			return res1.get_err();
		}
		auto res2 = phase3(expr->Right);
		if (res2.is_err()) {
			return res2.get_err();
		}
		auto& left_t = res1.get_ok();
		auto& right_t = res2.get_ok();
		auto param_t = type_cons::tuple(left_t, right_t);

		auto n_op_sym = SymTab.ref_sym("@op" + expr->Oper.Value);
		assert(n_op_sym);
		auto& op_sym = *n_op_sym;

		type* op_ty = nullptr;
		if (op_sym->Ty == symbol_t::Constant) {
			auto csym = (const_symbol*)op_sym;
			op_ty = csym->Type;
		}
		else if (op_sym->Ty == symbol_t::Variable) {
			UNREACHABLE;
		}
		else if (op_sym->Ty == symbol_t::Typeclass) {
			auto tsym = (typeclass_symbol*)op_sym;
			auto entry = unifier::add_class_constraint(
				Constraints, tsym, to_sem_pos(expr->Pos));
			op_ty = entry;
		}
		else {
			UNREACHABLE;
		}

		type* ret_t = new type_var();
		type* exp_func = type_cons::fn(param_t, ret_t);
		if (auto err = unifier::unify(op_ty, exp_func)) {
			return semantic_err(semantics_pos_err(
				"Semantic error: Wrong arguments provided for call",
				to_sem_pos(expr->Pos)
			));
		}
		return ret_t;
	}

	case AST_expr_t::IntLit: {
		// TODO: Better literals
		return I32;
	}

	case AST_expr_t::RealLit: {
		// TODO: Better literals
		return F32;
	}

	case AST_expr_t::BoolLit: {
		return BOOL;
	}

	default: UNIMPLEMENTED;
	}

	UNREACHABLE;
	return UNIT;
}
