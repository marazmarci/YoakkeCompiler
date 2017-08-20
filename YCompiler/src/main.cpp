#include <cstdlib>
#include <iostream>
#include "syntax/ast.h"
#include "io/file_hnd.h"
#include "io/fmt_code.h"
#include "syntax/lexer.h"
#include "syntax/token_input.h"
#include "functions.h"
#include "syntax/parser.h"
#include "io/ast_printer.h"
#include "syntax/ast_expr.h"
#include "syntax/ast_stmt.h"
#include "syntax/ast_ty.h"
#include "syntax/ast_pat.h"
#include "semantics/type.h"
#include "semantics/unifier.h"
#include "semantics/checker.h"

template <typename T>
void write_t(T const& t) {
	std::cout << typeid(t).name() << std::endl;
}

int main(void) {
	file_hnd file("C:/TMP/YoakkeTest/tokenizer.txt");
	if (!file.good()) {
		std::cout << "Could not open file: " << file.path() << '!' << std::endl;
	}
	lexer lex(file);
	token_input in(lex);

	auto res = parser::parse_program(in);
	if (res.is_err()) {
		parser::handle_error(res.get_err());
		std::cin.get();
		return 0;
	}
	auto& res_ok = res.get_ok();
	auto& st_list = std::get<0>(res_ok);
	for (auto& decl : st_list) {
		AST_printer::print(decl);
	}
	if (auto err = checker::check_program(file, st_list)) {
		checker::handle_error(*err);
		std::cin.get();
		return 0;
	}
	std::cout << "OK!" << std::endl;

	std::cin.get();
	return 0;
}
