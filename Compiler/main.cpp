#include <iostream>
#include "src/file_hnd.h"
#include "src/str_utils.h"
#include "src/lexer.h"

namespace dbg {
	void print_tok(token const& tok) {
		std::cout << '(' << tok.Pos.Start.Column
			<< ", " << tok.Pos.Start.Row << ") - "
			<< int(tok.Type) << " - '" << tok.Value << "'"
			<< std::endl;
	}
}

int main(void) {
	try {
		file_hnd	file("C:/TMP/YoakkeTest/tokenizer.txt");
		lexer		lex(file);
		while (lex.has_next()) {
			dbg::print_tok(lex.next());
		}
	}
	catch (no_file_exception& nof) {
		std::cout << "No such file!" << std::endl;
	}
	std::cin.get();
	return 0;
}
