/**
 * sem_checker.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  This file contains the semantic analyzer for the AST of Yoakke.
 */

#pragma once

#include "sym_tab.h"
#include "../syntax/position.h"

struct ty_symbol;
struct val_symbol;
struct AST_stmt;
struct AST_expr;
struct AST_ty;
struct AST_body_expr;
struct file_hnd;

struct undef_sym_exception {
public:
	file_hnd const& File;
	interval Pos;
	ystr Name;

public:
	undef_sym_exception(file_hnd const& f, interval const& pos, ystr const& name)
		: File(f), Pos(pos), Name(name) {
	}
};

/**
 * A semantic checker contains symbolic information and inserts them
 * based on the encountered nodes.
 */
struct sem_checker {
public:
	sem_checker(file_hnd const& file);

	void check(AST_stmt* st);
	ty_symbol* check(AST_expr* exp);
	ty_symbol* check(AST_ty* ty);
	ty_symbol* check(AST_body_expr* body);

private:
	void push_scope();
	void pop_scope();

private:
	file_hnd const& m_File;
	sym_tab<ty_symbol> m_Types;
	sym_tab<val_symbol> m_Values;

private:
	static ty_symbol* I32;
	static ty_symbol* BOOL;
	static ty_symbol* UNIT;
};
