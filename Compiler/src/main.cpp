#include <iostream>
#include "io/file_hnd.h"
#include "str_utils.h"
#include "syntax/lexer.h"
#include "io/code_formatter.h"
#include "syntax/parser.h"
#include "debug/ast_printer.h"

int main(void) {
	try {
		file_hnd	file("C:/TMP/YoakkeTest/tokenizer.txt");
		try {
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
		catch (lexer_eof_exception& eof) {
			std::cout << "Unexpected EOF in file: '" << eof.File.path() << "'."
				<< std::endl;
			code_formatter::print(eof.File, eof.Start, eof.End);
			std::cout << eof.Msg << std::endl;
			if (eof.Note) {
				std::cout << "Note: " << *eof.Note << std::endl;
			}
		}
		catch (lexer_unknown_exception& un) {
			std::cout << "Unknown token: '" 
				<< un.Tok << "' in file: '" << un.File.path() << "'."
				<< std::endl;
			code_formatter::print(un.File, un.Pos);
		}
		catch (parser_expect_exception& pe) {
			std::cout << "Syntax error in file: '" << pe.File.path() << "'."
				<< std::endl;
			code_formatter::print(pe.File, pe.Pos);
			std::cout << "Expected " << pe.Expected << ", but got "
				<< pe.Got << "!" << std::endl;
		}
	}
	catch (no_file_exception& nof) {
		std::cout << "No such file: '" << nof.Path << "'" << std::endl;
	}

	std::cin.get();
	return 0;
}
