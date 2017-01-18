#include <iostream>
#include "src\Parsing\Parser.h"

int main(void)
{
	yk::Parser parser;
	char src[] = "foo :: (a: i32, b: i32, c, d, e: i32) -> asd;";
	auto ast = parser.ParseProgram(src);

	for (auto t : ast)
	{
		std::cout << t->ToString();
	}
	std::cout << std::endl;

	system("PAUSE");
	return 0;
}