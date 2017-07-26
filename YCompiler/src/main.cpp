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

	auto LBracket = terminal<token_t::LBracket>();
	auto RBracket = terminal<token_t::RBracket>();
	auto Ident = terminal<token_t::Ident>();

	auto match = LBracket > Ident < RBracket;
	
	auto res = match(in);
	if (res.is_ok()) {
		auto& res_ok = res.get_ok();
		write_t(std::get<0>(res_ok));
	}
	else {
		std::cout << "No match!" << std::endl;
	}

	std::cin.get();
	return 0;
}
