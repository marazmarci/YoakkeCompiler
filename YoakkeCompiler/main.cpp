#include <fstream>
#include <iostream>
#include "src\Parsing\Parser.h"
#include "src\Semantics\SemanticChecker.h"
#include "src\Utils\FileUtils.h"
#include "src\Logging\Logger.h"
#include "src\IR\Unit.h"
#include "src\IR\Function.h"
#include "src\IR\BasicBlock.h"
#include "src\IR\IRPrinter.h"
#include "src\IR\Type.h"
#include "src\IR\Instruction.h"

int main(void)
{
	/*
	yk::Parser parser;
	yk::ystr const& src = yk::FileUtils::ReadFile("C:\\TMP\\YoakkeTest\\tokenizer.txt");
	auto ast = parser.ParseProgram(src.c_str());
	yk::SemanticChecker checker(src.c_str());
	checker.Check(ast);*/

	yk::IR::Unit* global = new yk::IR::Unit();
	auto i32 = new yk::IR::Integer(32);
	auto params = yk::yvec<yk::IR::Parameter>{
		yk::IR::Parameter("a", i32),
		yk::IR::Parameter("a", i32),
		yk::IR::Parameter("a", i32),
	};
	auto func = new yk::IR::Function("foo", i32, params);
	func->Add(new yk::IR::BasicBlock("main"));
	func->Add(new yk::IR::BasicBlock("asd"));
	func->Add(new yk::IR::BasicBlock("asd"));
	global->Add(func);

	for (yk::ysize i = 0; i < func->Blocks.size() - 1; i++)
	{
		func->Blocks[i]->Add(new yk::IR::JmpInstr(func->Blocks[i + 1]));
	}

	yk::IR::IRPrinter printer(std::cout);
	printer.Print(global);

	system("PAUSE");
	return 0;
}
