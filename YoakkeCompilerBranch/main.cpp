#include <iostream>
#include <string>
#include <fstream>

#include "src\parse\ylexer.h"
#include "src\parse\yparser.h"
#include "src\utility\static_block.h"
#include "src\utility\double_dispatcher.h"
#include "src\ast\expr.h"
#include "src\unit_testing\expr_unit_test.h"

yk::ystr read_file(yk::ystr const& fn) {
	std::ifstream t(fn);
	if (t.good()) {
		yk::ystr st((std::istreambuf_iterator<char>(t)),
			std::istreambuf_iterator<char>());
		return st;
	}
	return "";
}

int main(void) {
	yk::unit_testing::test_expr_parser();
	
	auto src = read_file("C:\\TMP\\YoakkeTest\\tokenizer.txt");
	yk::ylexer lexer;
	yk::token_buffer buffer(&lexer);
	yk::yparser parser(&buffer);
	lexer.set_source(src.c_str());
	buffer.clear();
	//try {
		auto exp = parser.parse_expr();
		auto exp2 = exp;
	//}
	//catch (std::exception exc) {
	//	std::cout << "Exception: " << exc.what() << std::endl;
	//}

	system("PAUSE");
	return 0;
}
