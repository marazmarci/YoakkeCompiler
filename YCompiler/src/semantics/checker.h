#pragma once

#include "class_constraint.h"
#include "semantic_pos.h"
#include "symbol_table.h"
#include "../syntax/token.h"
#include "../common.h"

struct AST_stmt;
struct AST_expr;
struct AST_ty;
struct AST_pat;
struct type_cons;
struct const_symbol;

struct semantics_def_err {
	const char*			Fmt;
	const char*			Kind;
	ystr				Name;
	yopt<semantic_pos>	DefPos;
	semantic_pos		RedefPos;

	semantics_def_err(const char* fmt, const char* kind, ystr const& name,
		yopt<semantic_pos> defpos, semantic_pos const& redefpos)
		: Fmt(fmt), Kind(kind), Name(name), 
		DefPos(std::move(defpos)), RedefPos(redefpos) {
	}
};

struct semantics_ty_err {
	const char*			Fmt;
	ystr				PrevName;
	ystr				CurrName;
	yopt<semantic_pos>	PrevPos;
	semantic_pos		CurrPos;

	semantics_ty_err(const char* fmt, ystr const& prn, ystr const& currn,
		yopt<semantic_pos> prevp, semantic_pos const& currp)
		: Fmt(fmt), PrevName(prn), CurrName(currn), 
		PrevPos(std::move(prevp)), CurrPos(currp) {
	}
};

struct semantics_pos_err {
	ystr			Msg;
	semantic_pos	Pos;

	semantics_pos_err(ystr const& msg, semantic_pos const& pos)
		: Msg(msg), Pos(pos) {
	}
};

using semantic_err = yvar<semantics_def_err, semantics_ty_err, semantics_pos_err>;

struct checker {
	static type_cons* UNIT;
	static type_cons* I32;
	static type_cons* F32;
	static type_cons* BOOL;

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

	yresult<type*, semantic_err> check_parameter(yopt<token>& m_name, AST_ty* ty_exp);
	yopt<semantic_err> decl_function(ystr const& name, const_symbol* sym, 
		semantic_pos const& name_pos);

	type* generate_let_pattern(AST_pat* pat, yvec<ytup<ystr, interval, type*>>& buff);
	ytup<yvec<ytup<ystr, interval, type*>>, type*> generate_let_pattern(AST_pat* pat);

	semantic_pos to_sem_pos(interval const& pos);

	static type* single_or(yvec<type*>& ts);

	static void print_def_msg(const char* fmt, const char* kind, ystr const& name,
		yopt<semantic_pos> defpos, semantic_pos const& redefpos);
	static void print_ty_msg(const char* fmt, ystr const& name1, ystr const& name2, 
		yopt<semantic_pos> const& pos1, semantic_pos const& pos2);
	static void print_pointed_msg(const char* msg, semantic_pos const& pos);

public:
	static void handle_error(semantic_err& err);
};
