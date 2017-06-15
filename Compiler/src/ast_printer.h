#pragma once

#include <iostream>
#include "ast_expr.h"

// TODO: comment

class ast_printer {
public:
	static std::ostream* Out;
	static ysize IndentWidth;

public:
	static void print(const AST_expr* exp);

private:
	static ysize s_Indent;

private:
	static void write_indent();
	static void indent_in();
	static void indent_out();

	static void tag_begin(ystr const& name);
	static void tag_end(ystr const& name);

public:
	ast_printer() = delete;
};
