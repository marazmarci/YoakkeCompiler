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

	int_expr(token const& tok) 
		: v(std::atoi(tok.Value.c_str())) {
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
		case::token_t::Mul: return a * b;
		case::token_t::Div: return a / b;
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
	
	auto IntLit = terminal<token_t::IntLit>();

	auto Add = terminal<token_t::Add>();
	auto Sub = terminal<token_t::Sub>();

	auto Mul = terminal<token_t::Mul>();
	auto Div = terminal<token_t::Div>();

	auto TransformLeftAssoc = [](auto& args) -> expr* {
		auto& left = std::get<0>(args);
		auto& rights = std::get<1>(args);
		for (auto it = rights.begin(); it != rights.end(); it++) {
			auto& elem = *it;
			auto& op = std::get<0>(elem);
			auto& right = std::get<1>(elem);
			left = new bin_expr(left, op, right);
		}
		return left;
	};

	auto Factor
		= (IntLit) ^ [](token& tok) -> expr* { return new int_expr(tok); };
	auto Term
		= (Factor >= (*((Mul | Div) >= Factor))) ^ TransformLeftAssoc;
	auto Expr
		= (Term >= (*((Add | Sub) >= Term))) ^ TransformLeftAssoc;

	auto res = Expr(in);
	if (res.is_ok()) {
		auto& res_ok = res.get_ok();
		auto& res_ninp = std::get<0>(res_ok);
		std::cout << res_ninp->eval() << std::endl;
	}
	else {
		std::cout << "No match!" << std::endl;
	}

	std::cin.get();
	return 0;
}
