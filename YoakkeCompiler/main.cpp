#include <iostream>
#include <fstream>
#include "src2\Debug\UnitTests\ParserUnitTest.h"
#include "src2\Parsing\YStmtParser.h"

using namespace yk;

ystr ReadFile(const char* name)
{
	std::ifstream t(name);
	if (t.good())
	{
		ystr str((std::istreambuf_iterator<char>(t)),
			std::istreambuf_iterator<char>());
		t.close();
		return str;
	}

	return "";
}

int main(void)
{
	dbg::Parser_UnitTest_All();

	auto src = ReadFile("C:\\TMP\\YoakkeTest\\tokenizer.txt");
	auto parser = yk::parse::YStmtParser(src.c_str(), "tokenizer.txt");
	auto ast = parser.ParseStmt();

	system("PAUSE");
	return 0;
}