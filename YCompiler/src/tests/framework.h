#pragma once

#include <functional>
#include "../common.h"

struct unit_test_case {
	ystr Name;
	std::function<void()> Func;

	unit_test_case(ystr const& name, std::function<void()> fn)
		: Name(name), Func(fn) {
	}
};

unit_test_case operator!=(ystr const& name, std::function<void()> fn);
void operator&(int, unit_test_case const& tc);

namespace unit_test {
	void run();

	namespace internal__ {
		void add_test(unit_test_case const& tc);
		void reset_counter();
		void add_ok();
		void add_err(bool succ, const char* what, const char* file, int line);
	}
}

#define TEST_CASE(msg, ...) 0 & msg != [__VA_ARGS__]()

#define TEST_SUCC(exp) if (exp) ::unit_test::internal__::add_ok(); else ::unit_test::internal__::add_err(true, #exp, __FILE__, __LINE__)
#define TEST_FAIL(exp) if (!(exp)) ::unit_test::internal__::add_ok(); else ::unit_test::internal__::add_err(false, #exp, __FILE__, __LINE__)
