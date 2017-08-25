#include <iostream>
#include "checker.h"
#include "symbol.h"
#include "symbol_table.h"
#include "type.h"
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
		if (auto n_ref = SymTab.local_ref_sym(name)) {
			auto& ref = *n_ref;
			if (ref->Ty == symbol_t::Variable) {
				return semantics_def_err(
					"Semantic error: %k %n shadows a variable %f!",
					"function", name, ref->DefPos, semantic_pos(File, stmt->Name.Pos)
				);
			}
		}
	}

	case AST_stmt_t::DbgWriteTy: {
		return {};
	}

	default: UNIMPLEMENTED;
	}

	UNREACHABLE;
}

yopt<semantic_err> checker::phase2(AST_expr* ex) {

}

yopt<semantic_err> checker::phase2(AST_ty* typ) {

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

void checker::handle_error(semantic_err& err) {
	fnl::match(err)(
		[](semantics_def_err& err) {
			print_def_msg(err.Fmt, err.Kind, err.Name, err.DefPos, err.RedefPos);
		}
	);
}
