#include <Windows.h>
#include <iostream>
#include "src\lexing\ylexer.h"
#include "src\parsing\yparser.h"

int main(void) {
	yk::yparser parser("C:/TMP/YoakkeTest/tokenizer.txt");
	auto exp = parser.parse_expr();

	system("PAUSE");
	return 0;
}
