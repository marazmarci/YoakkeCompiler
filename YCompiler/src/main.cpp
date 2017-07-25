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

int main(void) {
	file_hnd file("C:/TMP/YoakkeTest/tokenizer.txt");
	if (!file.good()) {
		std::cout << "Could not open file: " << file.path() << '!' << std::endl;
	}
	lexer lex(file);
	token_input in(lex);

	auto make_op = [](ytup<expr*, yvec<ytup<token, expr*>>>& input) -> expr* {
		auto left = std::get<0>(input);
		auto& right_rec = std::get<1>(input);

		for (auto it = right_rec.begin(); it != right_rec.end(); it++) {
			auto elem = *it;
			left = new bin_expr(left, std::get<0>(elem), std::get<1>(elem));
		}
		return left;
	};

	auto IntLit = combinator::wrap(
			[](auto res) -> expr* { return new int_expr(std::atoi(std::get<0>(res).Value.c_str())); },
			combinator::terminal<token_t::IntLit>()
		);
	auto Add = combinator::terminal<token_t::Add>();
	auto Sub = combinator::terminal<token_t::Sub>();
	auto LParen = combinator::terminal<token_t::LParen>();
	auto RParen = combinator::terminal<token_t::RParen>();

	auto BinOp = [=](auto res) {
		return combinator::wrap(make_op, res);
	};
	
	combinator::parser_t<expr*>& expr_rr;
	auto braced_r = LParen > expr_rr < RParen;

	std::cout << typeid(std::result_of_t<decltype(braced_r)(token_input&)>).name() << std::endl;

	combinator::parser_t<expr*> expr_r = IntLit;

	//auto res = matcher(in);
	//if (res.is_ok()) {
	//	auto& ok = res.get_ok();
	//	std::cout << std::get<0>(std::get<0>(ok))->eval() << std::endl;
	//}
	//else {
	//	std::cout << "No match!" << std::endl;
	//}

	std::cin.get();
	return 0;
}
