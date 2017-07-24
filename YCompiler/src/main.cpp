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
	
	auto matcher = combinator::sequence(
		combinator::terminal<token_t::LBracket>(),
		combinator::terminal<token_t::Ident>(),
		combinator::terminal<token_t::RBracket>()
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
