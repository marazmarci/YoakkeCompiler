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
				"No matching overload found",
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
			"Could not choose corresponding overload",
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
	case AST_expr_t::RealLit: {
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
	case AST_expr_t::RealLit: {
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
				ret_scope->ReturnType = ret_t;
				if (expr->Value) {
					auto& val = *expr->Value;
					ret_scope->ReturnPos = to_sem_pos(val->Pos);
				}
			}
		}
		else {
			auto n_ret_scope = SymTab.nearest_ret_dest();
			if (n_ret_scope) {
				auto ret_scope = *n_ret_scope;
				if (ret_scope->ReturnType) {
					ret_t = ret_scope->ReturnType;
				}
				else {
					ret_t = UNIT;
				}
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
		return expr->Symbol;
	}

	case AST_expr_t::If: {
		auto expr = (AST_if_expr*)ex;
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
		return UNIT;
	}

	case AST_expr_t::Let: UNIMPLEMENTED;

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
	case AST_expr_t::Bin:
	case AST_expr_t::Post: {
		// TODO
		UNIMPLEMENTED;
	}

	case AST_expr_t::IntLit: {
		// TODO: Better literals
		return I32;
	}

	case AST_expr_t::RealLit: {
		// TODO: Better literals
		return F32;
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
