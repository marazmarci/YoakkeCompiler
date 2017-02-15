#include <iostream>
#include <string>

#include "src\parse\lex_rule.h"

int main(void) {
	
	/*const char* src = "Hello there 234 654 123. .456 78.98";
	yk::ylexer lexer;
	lexer.set_source(src);
	while (lexer.has_next()) {
		auto tok = lexer.next();
		std::cout << tok.identifier() << " - " << tok.value() << std::endl;
	}*/

	auto seq = yk::lex_rule::chars("asd");
	auto mul = yk::lex_rule::mul(seq);
	auto seq2 = yk::lex_rule::chars("foo");
	auto opt = yk::lex_rule::mul(seq2);
	auto rule = mul;
	mul.Next = &opt;
	std::cout <<
		yk::lex_rule::match("asdasdasdfoo", rule)
		<< std::endl;

	system("PAUSE");
	return 0;
}
