#include <Windows.h>
#include <iostream>
#include <fstream>
#include "src\lexing\ylexer.h"
#include "src\parsing\yparser.h"
#include "src\utility\fmt_out.h"
#include "src\utility\fmt_in.h"
#include "src\parsing\parse_error.h"

int main(void) {
	try {
		yk::yparser parser("C:/TMP/YoakkeTest/tokenizer.txt");
		auto exp = parser.parse_expr();
	}
	catch (std::exception& ex) {
		std::cout << "Exception: " << ex.what() << std::endl;
	}

	std::ifstream test("C:/TMP/YoakkeTest/tokenizer.txt");
	std::cout << 
		yk::expect_parse_err("')'", "identifier", "C:/TMP/YoakkeTest/tokenizer.txt",
			yk::interval(yk::position(1, 15), 7))
		<< std::endl;

	system("PAUSE");
	return 0;
}
