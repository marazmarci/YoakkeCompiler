#include <iostream>
#include "checker.h"
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
}

yopt<semantic_err> checker::phase1(AST_stmt* st) {
	switch (st->Ty) {
	case AST_stmt_t::ConstDecl: UNIMPLEMENTED;

	case AST_stmt_t::Expr: {
		auto stmt = (AST_expr_stmt*)st;
		if (auto err = phase1(st)) {
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
			return semantics_already_def_err(
				"type", name, def->DefPos, semantic_pos(File, stmt->Name.Pos)
			);
		}
		if (auto m_def = SymTab.upper_ref_type(name)) {
			auto def = *m_def;
			print_shadow(
				"Warning: shadowing",
				"type", name, def->DefPos, semantic_pos(File, stmt->Name.Pos)
			);
		}
		stmt->Symbol = new type_var();
		SymTab.decl(name, stmt->Symbol);
		return {};
	}

	case AST_stmt_t::DbgWriteTy: {
		return;
	}

	default: UNIMPLEMENTED;
	}

	UNREACHABLE;
}

yopt<semantic_err> checker::phase1(AST_expr* ex) {

}

void checker::print_shadow(ystr const& report, ystr const& kind, ystr const& name,
	yopt<semantic_pos> defpos, semantic_pos const& redefpos) {
	auto& start = redefpos.Pos.Start;
	std::cout
		<< report << ' '
		<< kind << " '" << name << "' in file: '"
		<< redefpos.File.path()
		<< "', line " << start.Row
		<< ", character " << start.Column << '!'
		<< std::endl;
	if (defpos) {
		auto& ddefpos = *defpos;
		auto& defstart = ddefpos.Pos.Start;
		if (semantic_pos::same_file(ddefpos, redefpos)) {
			fmt_code::print(ddefpos.File, ddefpos.Pos, redefpos.Pos);
		}
		else {
			fmt_code::print(redefpos.File, redefpos.Pos);
			std::cout
				<< "Note: Previous definition is in file: '"
				<< ddefpos.File.path()
				<< "', line " << defstart.Row
				<< ", character " << defstart.Column << '.'
				<< std::endl;
			fmt_code::print(ddefpos.File, ddefpos.Pos);
		}
	}
}

void checker::handle_error(semantic_err& err) {
	fnl::match(err)(
		[](semantics_already_def_err& err) {
			print_shadow(
				"Semantic error: already defined",
				err.Kind, err.Name, err.DefPos, err.RedefPos
			);
		}
	);
}
