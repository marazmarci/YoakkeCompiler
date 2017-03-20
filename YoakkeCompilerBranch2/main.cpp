#include <Windows.h>
#include <iostream>
#include "src\lexing\ylexer.h"
#include "src\parsing\yparser.h"
#include <utility>
#include <type_traits>

int main(void) {
	try {
		yk::yparser parser("C:/TMP/YoakkeTest/tokenizer.txt");
		auto exp = parser.parse_expr();
	}
	catch (std::exception& ex) {
		std::cout << "Exception: " << ex.what() << std::endl;
	}

	system("PAUSE");
	return 0;
}
