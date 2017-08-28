#pragma once

#include "class_constraint.h"
#include "semantic_pos.h"
#include "symbol_table.h"
#include "../common.h"

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

struct semantics_ty_err {
	const char*			Fmt;
	ystr				Name1;
	ystr				Name2;
	semantic_pos		FirstPos;
	semantic_pos		SecondPos;

	semantics_ty_err(const char* fmt, ystr const& n1, ystr const& n2,
		semantic_pos const& p1, semantic_pos const& p2)
		: Fmt(fmt), Name1(n1), Name2(n2), FirstPos(p1), SecondPos(p2) {
	}
};

using semantic_err = yvar<semantics_def_err, semantics_ty_err>;

struct checker {
	static type_cons* UNIT;
	static type_cons* I32;
	static type_cons* F32;

	symbol_table			SymTab;
	file_hnd const&			File;
	yvec<class_constraint>	Constraints;

	checker(file_hnd const& f);

	yopt<semantic_err> check_program(yvec<AST_stmt*>& prg);

private:
	yopt<semantic_err> phase1(AST_stmt* st);
	yopt<semantic_err> phase1(AST_expr* ex);

	yopt<semantic_err> phase2(AST_stmt* st);
	yopt<semantic_err> phase2(AST_expr* ex);

	yopt<semantic_err> phase3(AST_stmt* st);
	yresult<type*, semantic_err> phase3(AST_expr* ex);

	yresult<type*, semantic_err> check_ty(AST_ty* typ);

	static type* single_or(yvec<type*>& ts);

	static void print_def_msg(const char* fmt, const char* kind, ystr const& name,
		yopt<semantic_pos> defpos, semantic_pos const& redefpos);
	static void print_ty_msg(const char* fmt, ystr const& name1,
		ystr const& name2, semantic_pos const& pos1, semantic_pos const& pos2);
	static void print_pointed_msg(const char* msg, semantic_pos const& pos);

public:
	static void handle_error(semantic_err& err);
};
