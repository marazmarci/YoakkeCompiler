#include <iostream>
#include <string>

#include "src\parse\ylexer.h"
#include "src\parse\yexpr_parser.h"
#include "src\utility\static_block.h"
#include "src\utility\double_dispatcher.h"
#include "src\ast\expr.h"

int main(void) {
	const char* src = "a + b";
	yk::ylexer lexer;
	lexer.set_source(src);
	yk::token_buffer buffer(&lexer);
	yk::yexpr_parser parser(&buffer);
	auto exp = parser.parse();

	system("PAUSE");
	return 0;
}
