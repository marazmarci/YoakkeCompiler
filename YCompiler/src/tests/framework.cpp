#include <iostream>
#include "framework.h"

unit_test_case operator!=(ystr const& name, std::function<void()> fn)
{
	return unit_test_case(name, fn);
}

void operator&(int, unit_test_case const& tc)
{
	unit_test::internal__::add_test(tc);
}

namespace {
	yvec<unit_test_case> Tests;
	unit_test_case* Current;
	ysize All;
	ysize Ok;
	ysize OkCases;
}

namespace unit_test {
	void run() {
		OkCases = 0;
		for (auto& t : Tests) {
			internal__::reset_counter();
			Current = &t;
			std::cout << "Test case: " << t.Name << std::endl;
			t.Func();
			std::cout << "Test case ";
			if (Ok == All) {
				std::cout << "succeeded";
				OkCases++;
			}
			else {
				std::cout << "failed";
			}
			std::cout << " (" << Ok << " / " << All << ')' << std::endl;
		}
		std::cout << "Unit test ended: " << OkCases << " / " << Tests.size() << std::endl;
		if (OkCases == Tests.size()) {
			std::cout << "PASSED" << std::endl;
		}
		else {
			std::cout << "FAILED" << std::endl;
		}
	}

	namespace internal__ {
		void add_test(unit_test_case const& tc) {
			Tests.push_back(tc);
		}

		void reset_counter() {
			All = 0;
			Ok = 0;
		}

		void add_ok() {
			All++;
			Ok++;
		}

		void add_err(bool succ, const char* what, const char* file, int line) {
			All++;
			if (succ) {
				std::cout << "Success test failed";
			}
			else {
				std::cout << "Fail test succeeded";
			}
			std::cout << " for: '" << what << "' in test case: '" 
				<< Current->Name << "' test: " << All << ", in file: " << file 
				<< ", line: " << line << std::endl;
		}
	}
}
