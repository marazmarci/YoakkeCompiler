#include <iostream>
#include <string>
#include <fstream>

#include "src\parse\ylexer.h"
#include "src\parse\yparser.h"
#include "src\utility\static_block.h"
#include "src\utility\double_dispatcher.h"
#include "src\ast\expr.h"
#include "src\unit_testing\expr_unit_test.h"
#include "src\semantics\semantic_checker.h"
#include "src\utility\unique_name_gen.h"
#include "src\ir\ir_printer.h"
#include "src\ir\ir_module.h"
#include "src\ir\ir_function.h"
#include "src\ir\ir_basic_block.h"
#include "src\ir\ir_instr.h"
#include "src\ir\ir_type.h"
#include "src\ir\ir_environment.h"
#include "src\platform\llvm_ir.h"
#include "src\ir\ir_value.h"

yk::ystr read_file(yk::ystr const& fn) {
	std::ifstream t(fn);
	if (t.good()) {
		yk::ystr st((std::istreambuf_iterator<char>(t)),
			std::istreambuf_iterator<char>());
		return st;
	}
	return "";
}

int main(void) {
	yk::unit_testing::test_expr_parser();
	
	auto src = read_file("C:\\TMP\\YoakkeTest\\tokenizer.txt");
	yk::ylexer lexer;
	yk::token_buffer buffer(&lexer);
	yk::yparser parser(&buffer);
	lexer.set_source(src.c_str());
	buffer.clear();
	yk::semantic_checker checker;
	try {
		yk::yvec<yk::stmt*> prog;
		while (yk::stmt* st = parser.parse_stmt()) prog.push_back(st);
		checker.check(prog);
	}
	catch (std::exception exc) {
		std::cout << "Exception: " << exc.what() << std::endl;
	}

	{
		using namespace yk;
		ir_environment::init();

		auto itype = new ir_int_type(32);
		ir_environment::add_type(itype);

		ir_module mod;
		auto fn = new ir_function("main", itype, yvec<ir_parameter*>{});
		auto bb = new ir_basic_block("entry");
		bb->add(new ir_ret_instr(new ir_int_value(32, 0)));
		fn->add(bb);
		mod.add(fn);

		std::ofstream ofile("C:\\TMP\\YoakkeTest\\llvm_test.txt");
		if (ofile.good()) {
			llvm_ir print(ofile);
			print.print(&mod);
			ofile.close();
		}
		else {
			std::cout << "Could not open file!" << std::endl;
		}

		ir_environment::deinit();
	}

	system("PAUSE");
	return 0;
}
