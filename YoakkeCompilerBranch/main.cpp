#include <iostream>
#include <string>

#include "src\parse\ylexer.h"

int main(void) {
	const char* src = "Hello there 234 654 123. .456 78.98";
	yk::ylexer lexer;
	lexer.set_source(src);
	while (lexer.has_next()) {
		auto tok = lexer.next();
		std::cout << tok.identifier() << " - " << tok.value() << std::endl;
	}

	system("PAUSE");
	return 0;
}
