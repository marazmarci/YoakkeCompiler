#include <cstdlib>
#include <iostream>
#include "syntax/ast.h"
#include "io/file_hnd.h"
#include "io/fmt_code.h"
#include "syntax/lexer.h"
#include "syntax/token_input.h"
#include "syntax/combinator.h"
#include "functions.h"
#include "syntax/parser.h"

template <typename T>
void write_t(T const& t) {
	std::cout << typeid(t).name() << std::endl;
}

int main(void) {
	using namespace parser;
	
	file_hnd file("C:/TMP/YoakkeTest/tokenizer.txt");
	if (!file.good()) {
		std::cout << "Could not open file: " << file.path() << '!' << std::endl;
	}
	lexer lex(file);
	token_input in(lex);

	parse_fn_expr(in);
	//auto res = p_label(in);
	//if (res.is_ok()) {
	//	auto& res_ok = res.get_ok();
	//	auto& lab = std::get<0>(res_ok);
	//	std::cout << lab.Name.Value << std::endl;
	//}
	//else {
	//	std::cout << "Not eof :(" << std::endl;
	//}

	std::cin.get();
	return 0;
}
