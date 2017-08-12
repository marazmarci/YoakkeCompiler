#include <cstdlib>
#include <iostream>
#include "syntax/ast.h"
#include "io/file_hnd.h"
#include "io/fmt_code.h"
#include "syntax/lexer.h"
#include "syntax/token_input.h"
#include "functions.h"
#include "syntax/parser.h"

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

	auto res = parser::parse_decl_stmt(in);
	if (res.is_ok()) {
		std::cout << "ok!" << std::endl;
	}
	else {
		parser::handle_error(res.get_err());
	}

	std::cin.get();
	return 0;
}
