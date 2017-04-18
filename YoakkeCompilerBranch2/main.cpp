#include <Windows.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include "src\lexing\ylexer.h"
#include "src\parsing\yparser.h"
#include "src\utility\fmt_out.h"
#include "src\utility\fmt_in.h"
#include "src\parsing\parse_error.h"
#include "src\debug\debug_printer.h"
#include "src\semantics\semantic_checker.h"

int main(void) {
	try {
		yk::yparser parser("C:/TMP/YoakkeTest/tokenizer.txt");
		yk::semantic_checker checker;

		std::chrono::time_point<std::chrono::system_clock> 
			start, parse_e, semantic_e;

		start = std::chrono::system_clock::now();
		
		auto prg = parser.parse_program();
		
		parse_e = std::chrono::system_clock::now();

		for (auto st : prg) {
			checker.check_stmt(st);
		}

		semantic_e = std::chrono::system_clock::now();

		std::chrono::duration<double> parse_t = parse_e - start;
		std::chrono::duration<double> semantic_t = semantic_e - parse_e;
		std::cout << "Parsing took: " << parse_t.count() << " s." << std::endl;
		std::cout << "Semantic analysis took: " << semantic_t.count() << " s." << std::endl;
	}
	catch (yk::yerror& err) {
		std::cout << err << std::endl;
	}
	catch (std::exception& ex) {
		std::cout << "Internal exception: " << ex.what() << std::endl;
	}
	catch (...) {
		std::cout << "Uncaught exception!" << std::endl;
	}

	system("PAUSE");
	return 0;
}
