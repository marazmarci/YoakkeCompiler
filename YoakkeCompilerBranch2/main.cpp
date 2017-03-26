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
	catch (yk::parse_err& err) {
		std::cout << err << std::endl;
	}
	catch (std::exception& ex) {
		std::cout << "Internal exception: " << ex.what() << std::endl;
	}
	catch (...) {
		std::cout << "Uncaught exception!" << std::endl;
	}

	system("PAUSE");
	return 0;
}
