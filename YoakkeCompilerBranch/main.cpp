#include <iostream>
#include <string>

#include "src\parse\ylexer.h"

int main(void) {
	const char* src = "Hello there";
	yk::ylexer lexer;
	lexer.set_source(src);
	while (lexer.has_next()) {
		auto tok = lexer.next();
		std::cout << tok.identifier() << " - " << tok.value() << std::endl;
	}

	system("PAUSE");
	return 0;
}
