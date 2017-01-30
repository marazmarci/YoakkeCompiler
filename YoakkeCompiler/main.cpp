#include <fstream>
#include <iostream>
#include "src\Parsing\Parser.h"
#include <Windows.h>
#include <stdio.h>

static std::string read_file(const char* name)
{
	std::ifstream t(name);
	if (t.good())
	{
		std::string str((std::istreambuf_iterator<char>(t)),
			std::istreambuf_iterator<char>());
		t.close();
		return str;
	}

	return "";
}

int main(void)
{
	yk::Parser parser;
	std::string const& src = read_file("C:\\TMP\\YoakkeTest\\tokenizer.txt");
	auto ast = parser.ParseProgram(src.c_str());

	for (auto t : ast)
	{
		std::cout << t->ToString() << std::endl;
	}
	std::cout << std::endl;

	system("PAUSE");
	return 0;
}
