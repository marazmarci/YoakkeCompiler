#include <cstdlib>
#include <iostream>
#include "io/file_hnd.h"
#include "io/fmt_code.h"
#include "syntax/lexer.h"
#include "syntax/token_input.h"
#include "syntax/combinator.h"

struct expr {
	virtual int eval() = 0;
};

struct int_expr : public expr {
	int v;

	int_expr(int i) 
		: v(i) {
	}

	int eval() override {
		return v;
	}
};

struct bin_expr : public expr {
	expr* l;
	expr* r;
	token op;

	bin_expr(expr* l, token op, expr* r)
		: l(l), r(r), op(op) {
	}

	int eval() override {
		int a = l->eval();
		int b = r->eval();
		switch (op.Type) {
		case::token_t::Add: return a + b;
		case::token_t::Sub: return a - b;
		}

		std::cout << "Error" << std::endl;
		return 0;
	}
};

template <typename T>
void write_t(T const& t) {
	std::cout << typeid(t).name() << std::endl;
}

int main(void) {
	file_hnd file("C:/TMP/YoakkeTest/tokenizer.txt");
	if (!file.good()) {
		std::cout << "Could not open file: " << file.path() << '!' << std::endl;
	}
	lexer lex(file);
	token_input in(lex);

	using namespace combinator;

	auto t1 = make_result_list(1, true);
	auto t2 = make_result_list(std::make_tuple(1, 2));
	auto t3 = make_result_list_from_tuple(std::make_tuple(1, 2));
	auto t4 = cat_result_lists(t2, t3);
	auto t5 = cat_result_lists(make_result_list(1, 2), make_result_list(true, false));
	auto t6 = cat_result_list_with_element(t5, 1.0f);
	auto t7 = cat_element_with_result_list(1.0f, t5);
	auto t8 = cat_results(1, true);
	auto t9 = cat_results(1, std::make_tuple());
	auto t10 = cat_results(t8, 1.0f);
	auto t11 = cat_results(1.0f, t8);
	auto t12 = cat_results(t10, t11);

	write_t(t1);
	write_t(t2);
	write_t(t3);
	write_t(t4);
	write_t(t5);
	write_t(t6);
	write_t(t7);
	write_t(t8);
	write_t(t9);
	write_t(t10);
	write_t(t11);
	write_t(t12);

	std::cin.get();
	return 0;
}
