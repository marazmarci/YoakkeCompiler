#include <iostream>
#include "src/io/file_hnd.h"
#include "src/str_utils.h"
#include "src/syntax/lexer.h"
#include "src/io/code_formatter.h"
#include "src/syntax/parser.h"
#include "src/debug/ast_printer.h"

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
		parser		par(lex);

		AST_expr* exp = par.parse_expr();
		if (exp) {
			AST_printer::print(exp);
		}
		else {
			std::cout << "No expression to print! :(" << std::endl;
		}
	}
	catch (no_file_exception& nof) {
		std::cout << "No such file: '" << nof.Path << "'" << std::endl;
	}

	std::cin.get();
	return 0;
}
