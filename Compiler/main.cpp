#include <iostream>
#include "src/file_hnd.h"
#include "src/str_utils.h"
#include "src/lexer.h"
#include "src/code_formatter.h"

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
		interval	tok_place(position(0, 0), position(1, 0));
		interval	tok_place2(position(0, 0), position(1, 0));
		file_hnd	file("C:/TMP/YoakkeTest/tokenizer.txt");
		lexer		lex(file);
		while (lex.has_next()) {
			token t = lex.next();
			if (t.Value == "Peter") {
				tok_place = t.Pos;
			}
			if (t.Type == token_t::Arrow) {
				tok_place2 = t.Pos;
			}
		}

		code_formatter::print(file, tok_place, tok_place2);
	}
	catch (no_file_exception& nof) {
		std::cout << "No such file: '" << nof.Path << "'" << std::endl;
	}
	std::cin.get();
	return 0;
}
