#include <iostream>
#include "src\Parsing\Parser.h"

int main(void)
{
	yk::Parser parser;
	char src[] = "2 3 4 post+ post+";
	auto ast = parser.ParseProgram(src);

	for (auto t : ast)
	{
		std::cout << t->ToString();
	}
	std::cout << std::endl;

	system("PAUSE");
	return 0;
}