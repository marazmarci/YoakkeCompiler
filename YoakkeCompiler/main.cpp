#include <fstream>
#include <iostream>
#include "src\Parsing\Parser.h"
#include "src\Semantics\SemanticChecker.h"
#include "src\Utils\FileUtils.h"
#include "src\Logging\Logger.h"

int main(void)
{
	yk::Parser parser;
	yk::ystr const& src = yk::FileUtils::ReadFile("C:\\TMP\\YoakkeTest\\tokenizer.txt");
	auto ast = parser.ParseProgram(src.c_str());
	yk::SemanticChecker checker(src.c_str());
	checker.Check(ast);

	system("PAUSE");
	return 0;
}
