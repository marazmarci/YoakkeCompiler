#include <fstream>
#include <iostream>
#include "src\Parsing\Parser.h"
#include "src\Semantics\SemanticChecker.h"

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
	yk::SemanticChecker checker;
	checker.Check(ast);

	system("PAUSE");
	return 0;
}
