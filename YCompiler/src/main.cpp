#include <cstdlib>
#include <iostream>
#include "syntax/ast.h"
#include "syntax/ast_stmt.h"
#include "io/file_hnd.h"
#include "syntax/token_input.h"
#include "syntax/parser.h"
#include "io/ast_printer.h"
#include "semantics/checker.h"
#include "tests/framework.h"
#include "tests/parser_tests.h"

template <typename T>
void write_t(T const& t) {
	std::cout << typeid(t).name() << std::endl;
}

int main(void) {
	add_parser_tests();
	unit_test::run();

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

	std::cout << "------------------------" << std::endl;

	checker check(file);
	if (auto err = check.check_program(st_list)) {
		checker::handle_error(*err);
	}
	else {
		std::cout << "Ok!" << std::endl;
	}

	std::cin.get();
	return 0;
}
