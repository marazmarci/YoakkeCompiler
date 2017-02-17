#include <iostream>
#include <string>

#include "src\parse\ylexer.h"
#include "src\parse\yexpr_parser.h"

int main(void) {
	const char* src = "foo hello there";
	yk::ylexer lexer;
	lexer.set_source(src);
	yk::token_buffer buffer(&lexer);
	yk::yexpr_parser parser(&buffer);
	auto exp = parser.parse();

	system("PAUSE");
	return 0;
}
