#pragma once

#include "../common.h"
#include "semantic_pos.h"
#include "symbol_table.h"

struct AST_stmt;
struct AST_expr;
struct AST_ty;
struct AST_pat;
struct type_cons;

struct semantics_def_err {
	const char*			Fmt;
	const char*			Kind;
	ystr				Name;
	yopt<semantic_pos>	DefPos;
	semantic_pos		RedefPos;

	semantics_def_err(const char* fmt, const char* kind, ystr const& name,
		yopt<semantic_pos> defpos, semantic_pos const& redefpos)
		: Fmt(fmt), Kind(kind), Name(name), DefPos(std::move(defpos)), RedefPos(redefpos) {
	}
};

using semantic_err = yvar<semantics_def_err>;

struct checker {
	static type_cons* UNIT;
	static type_cons* I32;
	static type_cons* F32;

	symbol_table	SymTab;
	file_hnd const& File;

	checker(file_hnd const& f);

	yopt<semantic_err> check_program(yvec<AST_stmt*>& prg);

private:
	yopt<semantic_err> phase1(AST_stmt* st);
	yopt<semantic_err> phase1(AST_expr* ex);

	yopt<semantic_err> phase2(AST_stmt* st);
	yopt<semantic_err> phase2(AST_expr* ex);
	yopt<semantic_err> phase2(AST_ty* typ);

	static void print_def_msg(const char* fmt, const char* kind, ystr const& name,
		yopt<semantic_pos> defpos, semantic_pos const& redefpos);

public:
	static void handle_error(semantic_err& err);
};
