#include <iostream>
#include "io/file_hnd.h"
#include "io/fmt_code.h"
#include "syntax/lexer.h"
#include "syntax/token_input.h"
#include "syntax/combinator.h"

int main(void) {
	file_hnd file("C:/TMP/YoakkeTest/tokenizer.txt");
	if (!file.good()) {
		std::cout << "Could not open file: " << file.path() << '!' << std::endl;
	}
	lexer lex(file);
	token_input in(lex);
	
	auto expr_m = combinator::terminal<token_t::IntLit>();
	auto op_m = combinator::either(
		combinator::terminal<token_t::Add>(),
		combinator::terminal<token_t::Sub>()
	);

	auto matcher = combinator::sequence(
		expr_m, op_m, expr_m
	);
	if (auto res = matcher(in)) {
		std::cout << "Match!" << std::endl;
	}
	else {
		std::cout << "No macth!" << std::endl;
	}

	std::cin.get();
	return 0;
}
