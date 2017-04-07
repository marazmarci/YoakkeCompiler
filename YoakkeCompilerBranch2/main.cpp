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

int main(void) {
	try {
		yk::yparser parser("C:/TMP/YoakkeTest/tokenizer.txt");
		std::chrono::time_point<std::chrono::system_clock> start, end;
		start = std::chrono::system_clock::now();
		auto prg = parser.parse_program();
		end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed = end - start;
		std::cout << "Parsing took: " << elapsed.count() << " s." << std::endl;
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
