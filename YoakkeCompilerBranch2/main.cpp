#include <Windows.h>
#include <iostream>
#include "src\lexing\ylexer.h"
#include "src\lexing\token.h"
#include "src\utility\gap_iterator.h"

int main(void) {
	/*yk::ylexer lexer("C:/TMP/YoakkeTest/tokenizer.txt");
	while (lexer.has_next()) {
		auto tok = lexer.next();
		std::cout << tok.Type << " - " << tok.Value << std::endl;
	}*/
	std::vector<int> testvec = {1,2,3,4,5,6,7,8,9,10,11,12};
	yk::gap_iterator<int> it(testvec);
	it.next();
	it.next();
	it.next();
	it.eat();
	it.next();
	it.next();
	it.next();

	system("PAUSE");
	return 0;
}
