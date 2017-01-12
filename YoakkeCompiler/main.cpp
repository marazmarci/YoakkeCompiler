#include <iostream>
#include "src\Parser.h"

int main(void)
{
	for (;;)
	{
		std::string line;
		std::getline(std::cin, line);
		yk::Parser parser;
		auto prog = parser.Parse(line);
		for (auto n : prog)
		{
			
		}
	}

	return 0;
}