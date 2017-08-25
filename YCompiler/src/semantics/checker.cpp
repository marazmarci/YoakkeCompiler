#include <iostream>
#include "checker.h"
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
type_cons* checker::I32 = new type_cons("i32");
type_cons* checker::F32 = new type_cons("f32");

checker::checker(file_hnd const& f)
	: File(f) {
}

yopt<semantic_err> checker::check_program(yvec<AST_stmt*>& prg) {
	SymTab.decl("unit", UNIT);
	SymTab.decl("i32", I32);
	SymTab.decl("f32", F32);

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
				"type", name, def->DefPos, semantic_pos(File, stmt->Name.Pos)
			);
		}
		if (auto m_def = SymTab.upper_ref_type(name)) {
			auto def = *m_def;
			print_def_msg(
				"Warning: shadowing %k %n %f!",
				"type", name, def->DefPos, semantic_pos(File, stmt->Name.Pos)
			);
		}
		stmt->Symbol = new type_var();
		stmt->Symbol->DefPos = semantic_pos(File, stmt->Name.Pos);
		SymTab.decl(name, stmt->Symbol);
		return {};
	}

	case AST_stmt_t::DbgWriteTy: {
		return {};
	}

	default: UNIMPLEMENTED;
	}

	UNREACHABLE;
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
		// Set up blocks
		if (expr->AsStatement) {
			expr->Then->AsStatement = true;
			if (expr->Else) {
				(*expr->Else)->AsStatement = true;
			}
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
		stmt->Symbol = type_cons::generic_fn();

		if (auto err = phase2(stmt->Expression)) {
			return err;
		}
		if (auto n_ref = SymTab.local_ref_sym(name)) {
			auto& ref = *n_ref;
			if (ref->Ty == symbol_t::Variable) {
				return semantics_def_err(
					"Semantic error: %k %n shadows a variable %f!",
					"function", name, ref->DefPos, semantic_pos(File, stmt->Name.Pos)
				);
			}
			else if (ref->Ty == symbol_t::Constant) {
				auto cons = (const_symbol*)ref;
				type* cons_t = cons->Type;
				if (auto err = unifier::unify(cons_t, type_cons::generic_fn())) {
					return semantics_def_err(
						"Semantic error: %k %n tries to overload a non-function constant %f!",
						"function", name, ref->DefPos, semantic_pos(File, stmt->Name.Pos)
					);
				}
				SymTab.remove_symbol(name);
				assert(!SymTab.local_ref_sym(name));

				type_cons* cons_tt = (type_cons*)cons_t;
				SymTab.decl(new typeclass_symbol(name, stmt->Symbol, cons_tt));
			}
			else if (ref->Ty == symbol_t::Typeclass) {
				auto tc = (typeclass_symbol*)ref;
				tc->add(stmt->Symbol);
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
					"function", name, ref->DefPos, semantic_pos(File, stmt->Name.Pos)
				);
			}
			SymTab.decl(new const_symbol(name, stmt->Symbol));
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
		if (auto err = phase2(expr->Body)) {
			return err;
		}
		SymTab.pop_scope();
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
}

// PHASE 3 ////////////////////////////////////////////////////////////////////

yopt<semantic_err> checker::phase3(AST_stmt* st) {
	switch (st->Ty) {
	case AST_stmt_t::ConstDecl: UNIMPLEMENTED;

	case AST_stmt_t::Expr: {
		auto stmt = (AST_expr_stmt*)st;
		auto res = phase3(stmt->Expression);
		if (res.is_err()) {
			return res.get_err();
		}
		auto ty = res.get_ok();
		if (!unifier::same(ty, UNIT)) {
			print_pointed_msg("Warning: ignoring return value!", 
				semantic_pos(File, stmt->Pos));
		}
		return {};
	}

	case AST_stmt_t::FnDecl: {
		auto stmt = (AST_fn_decl_stmt*)st;
		auto res = phase3(stmt->Expression);
		if (res.is_err()) {
			return res.get_err();
		}
		auto ty = res.get_ok();
		auto u_err = unifier::unify(stmt->Symbol, ty);
		assert(!u_err);
	}

	case AST_stmt_t::TyDecl: {
		return {};
	}

	case AST_stmt_t::DbgWriteTy: {
		auto stmt = (AST_dbg_write_ty_stmt*)st;
		auto res = phase3(stmt->Expression);
		if (res.is_err()) {
			return res.get_err();
		}
		std::cout << "DBG TY: " << unifier::to_str(res.get_ok()) << std::endl;
		return {};
	}

	default: UNIMPLEMENTED;
	}

	UNREACHABLE;
}

yresult<type*, semantic_err> checker::phase3(AST_expr* ex) {
	switch (ex->Ty) {
	case AST_expr_t::Block: {
		auto expr = (AST_block_expr*)ex;
		SymTab.push_scope(expr->Scope);
		for (auto& st : expr->Statements) {
			if (auto err = phase3(st)) {
				return *err;
			}
		}
		type* ret_ty = UNIT;
		if (expr->Value) {
			auto err = phase3(*expr->Value);
			if (err.is_err()) {
				return err.get_err();
			}
			ret_ty = err.get_ok();
		}
		SymTab.pop_scope();
		return ret_ty;
	}

	case AST_expr_t::Call: {
		auto expr = (AST_call_expr*)ex;
		auto res = phase3(expr->Func);
		if (res.is_err()) {
			return res.get_err();
		}
		auto fn_ty = res.get_ok();
		type* params_t = UNIT;
		if (expr->Params.size() == 1) {
			auto res2 = phase3(expr->Params[0]);
			if (res2.is_err()) {
				return res2.get_err();
			}
			params_t = res2.get_ok();
		}
		else if (expr->Params.size() > 1) {
			auto params_tt = type_cons::tuple();
			for (auto& param : expr->Params) {
				auto res2 = phase3(param);
				if (res2.is_err()) {
					return res.get_err();
				}
				params_tt->add(res2.get_ok());
			}
			params_t = params_tt;
		}
		type_var* ret_t = new type_var();
		auto ok_ty = type_cons::fn(params_t, ret_t);
		if (auto err = unifier::unify(fn_ty, ok_ty)) {
			return semantic_err(semantics_point_err(
				"Unable to call function!",
				expr->Func->Pos
			));
		}
		return ret_t;
	}

	case AST_expr_t::Fn: {
		auto expr = (AST_fn_expr*)ex;
		type* param_t = UNIT;
		if (expr->Params.size() == 1) {
			auto& param = expr->Params[0];
			auto& nname = std::get<0>(param);
			auto& ty_exp = std::get<1>(param);

			auto par_res = check_ty(ty_exp);
			if (par_res.is_err()) {
				return par_res.get_err();
			}
			param_t = par_res.get_ok();
			if (nname) {
				auto& name = *nname;
				SymTab.decl(name.Value, param_t);
			}
			else {
				print_pointed_msg(
					"Warning: unnamed parameter!",
					semantic_pos(File, ty_exp->Pos)
				);
			}
		}
		else if (expr->Params.size() > 1) {
			auto param_tt = type_cons::tuple();
			for (auto& param : expr->Params) {
				auto& nname = std::get<0>(param);
				auto& ty_exp = std::get<1>(param);

				auto par_res = check_ty(ty_exp);
				if (par_res.is_err()) {
					return par_res.get_err();
				}
				param_tt->add(par_res.get_ok());
				if (nname) {
					auto& name = *nname;
					SymTab.decl(name.Value, param_t);
				}
				else {
					print_pointed_msg(
						"Warning: unnamed parameter!",
						semantic_pos(File, ty_exp->Pos)
					);
				}
			}
			param_t = param_tt;
		}
		// TODO
		if (expr->Return) {

		}
	}

	case AST_expr_t::Pre:
	case AST_expr_t::Bin:
	case AST_expr_t::Post: {
		UNIMPLEMENTED;
	}

	case AST_expr_t::Ident: {
		auto expr = (AST_ident_expr*)ex;
		if (auto sym_n = SymTab.ref_sym(expr->Value)) {
			auto& sym = *sym_n;
			if (sym->Ty == symbol_t::Constant) {
				auto cs = (const_symbol*)sym;
				return cs->Type;
			}
			else if (sym->Ty == symbol_t::Variable) {
				auto vs = (var_symbol*)sym;
				return vs->Type;
			}
			else if (sym->Ty == symbol_t::Typeclass) {
				auto ts = (typeclass_symbol*)sym;
				return ts->Type;
			}
			else {
				UNIMPLEMENTED;
			}
		}
		else {
			return semantic_err(semantics_def_err(
				"Semantic error: Undefined %k %n %f!",
				"symbol", expr->Value, {}, semantic_pos(File, expr->Pos)
			));
		}
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
}

yresult<type*, semantic_err> checker::check_ty(AST_ty* typ) {
	switch (typ->Ty) {
	case AST_ty_t::Ident: {
		auto ty = (AST_ident_ty*)typ;
		if (auto t = SymTab.ref_type(ty->Value)) {
			return *t;
		}
		return semantic_err(semantics_def_err(
			"Semantic error: Undefined %k %n %f!",
			"type", ty->Value, {}, semantic_pos(File, ty->Pos)
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
}

///////////////////////////////////////////////////////////////////////////////

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

void checker::print_pointed_msg(const char* msg, semantic_pos const& pos) {
	std::cout << msg << std::endl;
	fmt_code::print(*pos.File, pos.Pos);
}

void checker::handle_error(semantic_err& err) {
	fnl::match(err)(
		[](semantics_def_err& err) {
			print_def_msg(err.Fmt, err.Kind, err.Name, err.DefPos, err.RedefPos);
		},
		[](semantics_point_err& err) {
			print_pointed_msg(err.Msg, err.Pos);
		}
	);
}
