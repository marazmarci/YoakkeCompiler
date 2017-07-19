#include <iostream>
#include "io/file_hnd.h"
#include "io/fmt_code.h"
#include "syntax/lexer.h"

int main(void) {
	file_hnd file("C:/TMP/YoakkeTest/tokenizer.txt");
	if (!file.good()) {
		std::cout << "Could not open file: " << file.path() << '!' << std::endl;
	}
	lexer lex(file);
	while (lex.has_next()) {
		auto res = lex.next();
		if (res.is_ok()) {
			token tok = res.get_ok();
			std::cout << tok << std::endl;
		}
		else {
			auto err = res.get_err();
			std::cout << "error" << std::endl;
		}
	}

	std::cin.get();
	return 0;
}
