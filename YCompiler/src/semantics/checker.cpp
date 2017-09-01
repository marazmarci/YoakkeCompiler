#include <iostream>
#include "checker.h"
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

type_cons* checker::UNIT = type_cons::tuple();
type_cons* checker::I32	 = new type_cons("i32");
type_cons* checker::F32	 = new type_cons("f32");
type_cons* checker::BOOL = new type_cons("bool");

checker::checker(file_hnd const& f)
	: File(f) {
}

yopt<semantic_err> checker::check_program(yvec<AST_stmt*>& prg) {
	SymTab.decl("unit", UNIT);
	SymTab.decl("i32",	I32);
	SymTab.decl("f32",	F32);
	SymTab.decl("bool", BOOL);

	for (auto& stmt : prg) {
		if (auto err = phase1(stmt)) {
			return err;
		}
	}
	for (auto& stmt : prg) {
		if (auto err = phase2(stmt)) {
			return err;
		}
	}
	for (auto& stmt : prg) {
		if (auto err = phase3(stmt)) {
			return err;
		}
	}

	while (true) {
		auto res = unifier::process_class_constraint_list(Constraints);
		if (res.is_err()) {
			auto& err = res.get_err();
			return semantics_pos_err(
				"Semantic error: No matching overload found",
				err.Position
			);
		}
		auto& ok = res.get_ok();
		if (!ok) {
			break;
		}
	}
	if (Constraints.size()) {
		// TODO: Could report all, just report first for now
		return semantic_err(semantics_pos_err(
			"Semantic error: Could not choose corresponding overload",
			Constraints[0].Position
		));
	}

	return {};
}

// PHASE 1 ////////////////////////////////////////////////////////////////////

yopt<semantic_err> checker::phase1(AST_stmt* st) {
	switch (st->Ty) {
	case AST_stmt_t::ConstDecl: UNIMPLEMENTED;

	case AST_stmt_t::Expr: {
		auto stmt = (AST_expr_stmt*)st;
		if (auto err = phase1(stmt->Expression)) {
			return err;
		}
		return {};
	}

	case AST_stmt_t::FnDecl: {
		auto stmt = (AST_fn_decl_stmt*)st;
		if (auto err = phase1(stmt->Expression)) {
			return err;
		}
		return {};
	}

	case AST_stmt_t::OpDecl: {
		auto stmt = (AST_op_decl_stmt*)st;
		if (auto err = phase1(stmt->Expression)) {
			return err;
		}
		return {};
	}

	case AST_stmt_t::TyDecl: {
		auto stmt = (AST_ty_decl_stmt*)st;
		ystr const& name = stmt->Name.Value;
		if (auto m_def = SymTab.local_ref_type(name)) {
			auto def = *m_def;
			return semantics_def_err(
				"Semantic error: %k %n already defined %f!",
				"type", name, def->DefPos, to_sem_pos(stmt->Name.Pos)
			);
		}
		if (auto m_def = SymTab.upper_ref_type(name)) {
			auto def = *m_def;
			print_def_msg(
				"Warning: shadowing %k %n %f!",
				"type", name, def->DefPos, to_sem_pos(stmt->Name.Pos)
			);
		}
		stmt->Symbol = new type_var();
		stmt->Symbol->DefPos = to_sem_pos(stmt->Name.Pos);
		SymTab.decl(name, stmt->Symbol);
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

yopt<semantic_err> checker::phase1(AST_expr* ex) {
	switch (ex->Ty) {
	case AST_expr_t::Block: {
		auto expr = (AST_block_expr*)ex;
		expr->Scope = SymTab.push_scope(!expr->AsStatement);
		for (auto& st : expr->Statements) {
			if (auto err = phase1(st)) {
				return err;
			}
		}
		if (expr->Value) {
			if (auto err = phase1(*expr->Value)) {
				return err;
			}
		}
		SymTab.pop_scope();
		return {};
	}

	case AST_expr_t::Call: {
		auto expr = (AST_call_expr*)ex;
		if (auto err = phase1(expr->Func)) {
			return err;
		}
		for (auto& param : expr->Params) {
			if (auto err = phase1(param)) {
				return err;
			}
		}
		return {};
	}

	case AST_expr_t::Fn: {
		auto expr = (AST_fn_expr*)ex;
		expr->Scope = SymTab.push_scope(true);
		if (auto err = phase1(expr->Body)) {
			return err;
		}
		SymTab.pop_scope();
		return {};
	}

	case AST_expr_t::If: {
		auto expr = (AST_if_expr*)ex;
		expr->Scope = SymTab.push_scope(!expr->AsStatement);
		if (auto err = phase1(expr->Condition)) {
			return err;
		}
		if (auto err = phase1(expr->Then)) {
			return err;
		}
		if (expr->Else) {
			if (auto err = phase1(*expr->Else)) {
				return err;
			}
		}
		SymTab.pop_scope();
		return {};
	}

	case AST_expr_t::Let: {
		auto expr = (AST_let_expr*)ex;
		if (expr->Value) {
			if (auto err = phase1(*expr->Value)) {
				return err;
			}
		}
		return {};
	}

	case AST_expr_t::List: {
		auto expr = (AST_list_expr*)ex;
		for (auto& exp : expr->Elements) {
			if (auto err = phase1(exp)) {
				return err;
			}
		}
		return {};
	}

	case AST_expr_t::Pre: {
		auto expr = (AST_pre_expr*)ex;
		if (auto err = phase1(expr->Subexpr)) {
			return err;
		}
		return {};
	}

	case AST_expr_t::Bin: {
		auto expr = (AST_bin_expr*)ex;
		if (auto err = phase1(expr->Left)) {
			return err;
		}
		if (auto err = phase1(expr->Right)) {
			return err;
		}
		return {};
	}

	case AST_expr_t::Post: {
		auto expr = (AST_post_expr*)ex;
		if (auto err = phase1(expr->Subexpr)) {
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

// PHASE 2 ////////////////////////////////////////////////////////////////////

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
		sym->DefPos = to_sem_pos(stmt->Name.Pos);
		if (auto n_ref = SymTab.local_ref_sym(name)) {
			auto& ref = *n_ref;
			if (ref->Ty == symbol_t::Variable) {
				return semantics_def_err(
					"Semantic error: %k %n shadows a variable %f!",
					"function", name, ref->DefPos, to_sem_pos(stmt->Name.Pos)
				);
			}
			else if (ref->Ty == symbol_t::Constant) {
				auto cons = (const_symbol*)ref;
				type* cons_t = cons->Type;
				if (auto err = unifier::unify(cons_t, type_cons::generic_fn())) {
					return semantics_def_err(
						"Semantic error: %k %n tries to overload a non-function constant %f!",
						"function", name, ref->DefPos, to_sem_pos(stmt->Name.Pos)
					);
				}
				SymTab.remove_symbol(name);
				assert(!SymTab.local_ref_sym(name));

				auto tc_sym = new typeclass_symbol(name, cons);
				if (auto n_other = tc_sym->add(sym)) {
					auto& other = *n_other;
					return semantics_def_err(
						"Semamtic error: %k %n cannot overload a matching function %f!",
						"function", name, other->DefPos, to_sem_pos(stmt->Name.Pos)
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
						"function", name, other->DefPos, to_sem_pos(stmt->Name.Pos)
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
					"function", name, ref->DefPos, to_sem_pos(stmt->Name.Pos)
				);
			}
			SymTab.decl(sym);
		}
		return {};
	}

	// TODO: Common code with FnDecl
	case AST_stmt_t::OpDecl: {
		auto stmt = (AST_op_decl_stmt*)st;
		ystr const& name = "@op" + stmt->Operator; // DIFFERENT
		if (auto err = phase2(stmt->Expression)) {
			return err;
		}
		auto& sym_t = stmt->Expression->Symbol;
		// NEW ///////////////////////////////////////////
		{
			// TODO: Can speed up overloaded stuff by adding the no. operands to the name
			//  Would also work with non-operators, like foo@2, foo@3, ...
			// TODO: For now only 2 params, but need unary and others too
			if (true) {	// Operator is binary
				auto two_params = type_cons::tuple(new type_var(), new type_var());
				if (auto err = unifier::unify(sym_t->Params[0], two_params)) {
					return semantics_pos_err(
						"Semantic error: Wrong number of arguments for binary operator " + stmt->Operator,
						to_sem_pos(stmt->Pos)
					);
				}
			}
		}
		//////////////////////////////////////////////////
		auto sym = new const_symbol(name, sym_t);
		sym->DefPos = to_sem_pos(stmt->Pos);		// DIFFERENT
		if (auto n_ref = SymTab.local_ref_sym(name)) {
			auto& ref = *n_ref;
			if (ref->Ty == symbol_t::Variable) {
				return semantics_def_err(
					"Semantic error: %k %n shadows a variable %f!",
					"function", name, ref->DefPos, to_sem_pos(stmt->Pos) // DIFFERENT
				);
			}
			else if (ref->Ty == symbol_t::Constant) {
				auto cons = (const_symbol*)ref;
				type* cons_t = cons->Type;
				if (auto err = unifier::unify(cons_t, type_cons::generic_fn())) {
					return semantics_def_err(
						"Semantic error: %k %n tries to overload a non-function constant %f!",
						"function", name, ref->DefPos, to_sem_pos(stmt->Pos) // DIFFERENT
					);
				}
				SymTab.remove_symbol(name);
				assert(!SymTab.local_ref_sym(name));

				auto tc_sym = new typeclass_symbol(name, cons);
				if (auto n_other = tc_sym->add(sym)) {
					auto& other = *n_other;
					return semantics_def_err(
						"Semamtic error: %k %n cannot overload a matching function %f!",
						"function", name, other->DefPos, to_sem_pos(stmt->Pos) // DIFFERENT
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
						"function", name, other->DefPos, to_sem_pos(stmt->Pos) // DIFFERENT
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
					"function", name, ref->DefPos, to_sem_pos(stmt->Pos) // DIFFERENT
				);
			}
			SymTab.decl(sym);
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
		expr->Symbol = type_cons::fn(single_or(param_list), ret_t);
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

// PHASE 3 ////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

yresult<type*, semantic_err> checker::check_ty(AST_ty* typ) {
	switch (typ->Ty) {
	case AST_ty_t::Ident: {
		auto ty = (AST_ident_ty*)typ;
		if (auto t = SymTab.ref_type(ty->Value)) {
			return *t;
		}
		return semantic_err(semantics_def_err(
			"Semantic error: Undefined %k %n %f!",
			"type", ty->Value, {}, to_sem_pos(ty->Pos)
		));
	}

	case AST_ty_t::List: {
		auto ty = (AST_list_ty*)typ;
		auto ty_sym = type_cons::tuple();
		for (auto& t : ty->Elements) {
			auto res = check_ty(t);
			if (res.is_err()) {
				return res.get_err();
			}
			ty_sym->add(res.get_ok());
		}
		return ty_sym;
	}

	case AST_ty_t::Bin: {
		auto ty = (AST_bin_ty*)typ;
		assert(ty->Oper.Type == token_t::Arrow);
		auto res_l = check_ty(ty->Left);
		if (res_l.is_err()) {
			return res_l.get_err();
		}
		auto res_r = check_ty(ty->Right);
		if (res_r.is_err()) {
			return res_r.get_err();
		}
		return type_cons::fn(res_l.get_ok(), res_r.get_ok());
	}

	case AST_ty_t::Pre:
	case AST_ty_t::Post: UNIMPLEMENTED;

	default: UNIMPLEMENTED;
	}

	UNREACHABLE;
	return UNIT;
}

type* checker::generate_let_pattern(AST_pat* pat, yvec<ytup<ystr, interval, type*>>& buff) {
	switch (pat->Ty) {
	case AST_pat_t::List: {
		auto pt = (AST_list_pat*)pat;
		type_cons* tlist = new type_cons(type_prefixes::Tuple);
		for (auto& elem : pt->Elements) {
			tlist->add(generate_let_pattern(elem, buff));
		}
		return tlist;
	}

	case AST_pat_t::Ident: {
		auto pt = (AST_ident_pat*)pat;
		type_var* tv = new type_var();
		buff.push_back({ pt->Value, pt->Pos, tv });
		return tv;
	}

	case AST_pat_t::Bin:
	case AST_pat_t::Pre:
	case AST_pat_t::Post: {
		UNIMPLEMENTED;
	}

	default: UNIMPLEMENTED;
	}

	UNREACHABLE;
	return nullptr;
}

ytup<yvec<ytup<ystr, interval, type*>>, type*> checker::generate_let_pattern(AST_pat* pat) {
	yvec<ytup<ystr, interval, type*>> buff;
	type* ty = generate_let_pattern(pat, buff);
	return { buff, ty };
}

///////////////////////////////////////////////////////////////////////////////

semantic_pos checker::to_sem_pos(interval const& pos) {
	return semantic_pos(File, pos);
}

type* checker::single_or(yvec<type*>& ts) {
	if (ts.size() == 1) {
		return *ts.begin();
	}
	return new type_cons(type_prefixes::Tuple, ts);
}

// TODO: Unifier formatting
void checker::print_def_msg(const char* fmt, const char* kind, ystr const& name,
	yopt<semantic_pos> defpos, semantic_pos const& redefpos) {
	auto& start = redefpos.Pos.Start;
	const char* ptr = fmt;
	while (char c = *ptr++) {
		if (c == '%') {
			c = *ptr++;
			switch (c) {
			case '%': {
				std::cout << '%';
				break;
			}

			case 'f': {
				std::cout << "in file: '"
					<< redefpos.File->path()
					<< "', line " << start.Row
					<< ", character " << start.Column;
				break;
			}

			case 'k': {
				std::cout << kind;
				break;
			}

			case 'n': {
				std::cout << '\'' << name << '\'';
				break;
			}

			default: UNIMPLEMENTED;
			}
		}
		else {
			std::cout << c;
		}
	}
	std::cout << std::endl;
	if (defpos) {
		auto& ddefpos = *defpos;
		auto& defstart = ddefpos.Pos.Start;
		if (semantic_pos::same_file(ddefpos, redefpos)) {
			fmt_code::print(*ddefpos.File, ddefpos.Pos, redefpos.Pos);
		}
		else {
			fmt_code::print(*redefpos.File, redefpos.Pos);
			std::cout
				<< "Note: Previous definition is in file: '"
				<< ddefpos.File->path()
				<< "' at line " << defstart.Row
				<< ", character " << defstart.Column << '.'
				<< std::endl;
			fmt_code::print(*ddefpos.File, ddefpos.Pos);
		}
	}
	else {
		fmt_code::print(*redefpos.File, redefpos.Pos);
	}
}

void checker::print_ty_msg(const char* fmt, ystr const& name1, ystr const& name2, 
	yopt<semantic_pos> const& pos1, semantic_pos const& pos2) {
	auto& start = pos2.Pos.Start;
	const char* ptr = fmt;
	while (char c = *ptr++) {
		if (c == '%') {
			c = *ptr++;
			switch (c) {
			case '%': {
				std::cout << '%';
				break;
			}

			case 'f': {
				std::cout << "in file: '"
					<< pos2.File->path()
					<< "', line " << start.Row
					<< ", character " << start.Column;
				break;
			}

			case 'a': {
				std::cout << '\'' << name1 << '\'';
				break;
			}

			case 'b': {
				std::cout << '\'' << name2 << '\'';
				break;
			}

			default: UNIMPLEMENTED;
			}
		}
		else {
			std::cout << c;
		}
	}
	std::cout << std::endl;
	if (pos1) {
		auto& ppos1 = *pos1;
		if (semantic_pos::same_file(ppos1, pos2)) {
			fmt_code::print(*ppos1.File, ppos1.Pos, pos2.Pos);
		}
		else {
			fmt_code::print(*pos2.File, pos2.Pos);
		}
	}
	else {
		fmt_code::print(*pos2.File, pos2.Pos);
	}
}

void checker::print_pointed_msg(const char* msg, semantic_pos const& pos) {
	auto& start = pos.Pos.Start;
	std::cout
		<< msg
		<< " in file: '"
		<< pos.File->path()
		<< "', line " << start.Row
		<< ", character " << start.Column
		<< '!' << std::endl;
	fmt_code::print(*pos.File, pos.Pos);
}

void checker::handle_error(semantic_err& err) {
	fnl::match(err)(
		[](semantics_def_err& err) {
			print_def_msg(err.Fmt, err.Kind, err.Name, err.DefPos, err.RedefPos);
		},
		[](semantics_ty_err& err) {
			print_ty_msg(err.Fmt, err.PrevName, err.CurrName, err.PrevPos, err.CurrPos);
		},
		[](semantics_pos_err& err) {
			print_pointed_msg(err.Msg.c_str(), err.Pos);
		}
	);
}
