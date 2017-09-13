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

yopt<semantic_err> checker::decl_function(ystr const& name, const_symbol* sym, 
	semantic_pos const& name_pos) {
	if (auto n_ref = SymTab.local_ref_sym(name)) {
		auto& ref = *n_ref;
		if (ref->Ty == symbol_t::Variable) {
			return semantics_def_err(
				"Semantic error: %k %n shadows a variable %f!",
				"function", name, ref->DefPos, name_pos
			);
		}
		else if (ref->Ty == symbol_t::Constant) {
			auto cons = (const_symbol*)ref;
			type* cons_t = cons->Type;
			if (auto err = unifier::unify(cons_t, type_cons::generic_fn())) {
				return semantics_def_err(
					"Semantic error: %k %n tries to overload a non-function constant %f!",
					"function", name, ref->DefPos, name_pos
				);
			}
			SymTab.remove_symbol(name);
			assert(!SymTab.local_ref_sym(name));

			auto tc_sym = new typeclass_symbol(name, cons);
			if (auto n_other = tc_sym->add(sym)) {
				auto& other = *n_other;
				return semantics_def_err(
					"Semamtic error: %k %n cannot overload a matching function %f!",
					"function", name, other->DefPos, name_pos
				);
			}
			SymTab.decl(tc_sym);
		}
		else if (ref->Ty == symbol_t::Typeclass) {
			auto tc = (typeclass_symbol*)ref;
			if (auto n_other = tc->add(sym)) {
				auto& other = *n_other;
				return semantics_def_err(
					"Semamtic error: %k %n cannot overload a matching function %f!",
					"function", name, other->DefPos, name_pos
				);
			}
		}
		else {
			UNREACHABLE;
		}
	}
	else {
		if (auto n_ref = SymTab.upper_ref_sym(name)) {
			auto& ref = *n_ref;
			print_def_msg(
				"Warning: %k %n is shadowing other functions or constants %f!",
				"function", name, ref->DefPos, name_pos
			);
		}
		SymTab.decl(sym);
	}
	return {};
}

yopt<semantic_err> checker::phase2(AST_stmt* st) {
	switch (st->Ty) {
	case AST_stmt_t::ConstDecl: UNIMPLEMENTED;

	case AST_stmt_t::Expr: {
		auto stmt = (AST_expr_stmt*)st;
		if (auto err = phase2(stmt->Expression)) {
			return err;
		}
		return {};
	}

	case AST_stmt_t::FnDecl: {
		auto stmt = (AST_fn_decl_stmt*)st;
		ystr const& name = stmt->Name.Value;
		if (auto err = phase2(stmt->Expression)) {
			return err;
		}
		auto& sym_t = stmt->Expression->Symbol;
		auto sym = new const_symbol(name, sym_t);
		auto pos = to_sem_pos(stmt->Name.Pos);
		sym->DefPos = pos;
		if (auto err = decl_function(name, sym, pos)) {
			return *err;
		}
		return {};
	}

	case AST_stmt_t::OpDecl: {
		auto stmt = (AST_op_decl_stmt*)st;
		ystr name = "@op" + stmt->Operator.Value;
		if (auto err = phase2(stmt->Expression)) {
			return err;
		}
		auto& sym_t = stmt->Expression->Symbol;
		// NEW ///////////////////////////////////////////
		// TODO: Can speed up overloaded stuff by adding the no. operands to the name
		//  Would also work with non-operators, like foo@2, foo@3, ...
		if (!oper_desc::good_def(stmt->Operator.Type, sym_t->Params[0])) {
			return semantics_pos_err(
				"Semantic error: Wrong number of arguments for operator" +
				stmt->Operator.Value,
				to_sem_pos(stmt->NamePos)
			);
		}
		//////////////////////////////////////////////////
		auto sym = new const_symbol(name, sym_t);
		auto pos = to_sem_pos(stmt->NamePos);
		sym->DefPos = pos;
		if (auto err = decl_function(name, sym, pos)) {
			return *err;
		}
		return {};
	}

	case AST_stmt_t::TyDecl: {
		auto stmt = (AST_ty_decl_stmt*)st;
		auto res = check_ty(stmt->Type);
		if (res.is_err()) {
			return res.get_err();
		}
		auto ty = res.get_ok();
		auto u_err = unifier::unify(stmt->Symbol, ty);
		assert(!u_err);
		return {};
	}

	case AST_stmt_t::DbgWriteTy: {
		return {};
	}

	default: UNIMPLEMENTED;
	}

	UNREACHABLE;
	return {};
}

yresult<type*, semantic_err> checker::check_parameter(yopt<token>& m_name, AST_ty* ty_exp) {
	auto res = check_ty(ty_exp);
	if (res.is_err()) {
		return res.get_err();
	}
	type* param_t = res.get_ok();
	if (m_name) {
		auto& name = *m_name;
		if (auto m_def = SymTab.local_ref_sym(name.Value)) {
			auto& def = *m_def;
			return semantic_err(semantics_def_err(
				"Semantic error: %k %n already defined %f!",
				"parameter", name.Value, def->DefPos, to_sem_pos(name.Pos)
			));
		}
		var_symbol* param = new var_symbol(name.Value, param_t);
		param->DefPos = to_sem_pos(name.Pos);
		SymTab.decl(param);
	}
	else {
		print_pointed_msg(
			"Warning: Unnamed parameter", to_sem_pos(ty_exp->Pos)
		);
	}
	return param_t;
}

yopt<semantic_err> checker::phase2(AST_expr* ex) {
	switch (ex->Ty) {
	case AST_expr_t::Block: {
		auto expr = (AST_block_expr*)ex;
		SymTab.push_scope(expr->Scope);
		for (auto& st : expr->Statements) {
			if (auto err = phase2(st)) {
				return err;
			}
		}
		if (expr->Value) {
			if (auto err = phase2(*expr->Value)) {
				return err;
			}
		}
		SymTab.pop_scope();
		return {};
	}

	case AST_expr_t::Call: {
		auto expr = (AST_call_expr*)ex;
		if (auto err = phase2(expr->Func)) {
			return err;
		}
		for (auto& param : expr->Params) {
			if (auto err = phase2(param)) {
				return err;
			}
		}
		return {};
	}

	case AST_expr_t::Fn: {
		auto expr = (AST_fn_expr*)ex;
		SymTab.push_scope(expr->Scope);
		yvec<type*> param_list;
		for (auto& param_tup : expr->Params) {
			auto& m_name = std::get<0>(param_tup);
			auto& ty_exp = std::get<1>(param_tup);

			auto res = check_parameter(m_name, ty_exp);
			if (res.is_err()) {
				return res.get_err();
			}
			param_list.push_back(res.get_ok());
		}
		type* ret_t = UNIT;
		if (expr->Return) {
			auto& ret_node = *expr->Return;
			auto res = check_ty(ret_node);
			if (res.is_err()) {
				return res.get_err();
			}
			ret_t = res.get_ok();
			expr->Scope->ReturnPos = to_sem_pos(ret_node->Pos);
		}
		expr->Scope->ReturnType = ret_t;
		if (auto err = phase2(expr->Body)) {
			return err;
		}
		SymTab.pop_scope();
		expr->Symbol = type_cons::fn(type_cons::params(param_list), ret_t);
		return {};
	}

	case AST_expr_t::If: {
		auto expr = (AST_if_expr*)ex;
		SymTab.push_scope(expr->Scope);
		if (auto err = phase2(expr->Condition)) {
			return err;
		}
		if (auto err = phase2(expr->Then)) {
			return err;
		}
		if (expr->Else) {
			if (auto err = phase2(*expr->Else)) {
				return err;
			}
		}
		SymTab.pop_scope();
		return {};
	}

	case AST_expr_t::Let: {
		auto expr = (AST_let_expr*)ex;
		if (expr->Value) {
			if (auto err = phase2(*expr->Value)) {
				return err;
			}
		}
		return {};
	}

	case AST_expr_t::List: {
		auto expr = (AST_list_expr*)ex;
		for (auto& exp : expr->Elements) {
			if (auto err = phase2(exp)) {
				return err;
			}
		}
		return {};
	}

	case AST_expr_t::Pre: {
		auto expr = (AST_pre_expr*)ex;
		if (auto err = phase2(expr->Subexpr)) {
			return err;
		}
		return {};
	}

	case AST_expr_t::Bin: {
		auto expr = (AST_bin_expr*)ex;
		if (auto err = phase2(expr->Left)) {
			return err;
		}
		if (auto err = phase2(expr->Right)) {
			return err;
		}
		return {};
	}

	case AST_expr_t::Post: {
		auto expr = (AST_post_expr*)ex;
		if (auto err = phase2(expr->Subexpr)) {
			return err;
		}
		return {};
	}

	case AST_expr_t::Ident:
	case AST_expr_t::IntLit:
	case AST_expr_t::RealLit:
	case AST_expr_t::BoolLit: {
		return {};
	}

	default: UNIMPLEMENTED;
	}

	UNREACHABLE;
	return {};
}