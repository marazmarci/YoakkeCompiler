#include <iostream>
#include <fstream>
#include "src2\Debug\ParserTest.h"

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
	//auto src = ReadFile("C:\\TMP\\YoakkeTest\\tokenizer.txt");
	dbg::Parser_TestAll();

	system("PAUSE");
	return 0;
}