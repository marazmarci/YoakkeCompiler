#include <cstdlib>
#include <iostream>
#include "io/file_hnd.h"
#include "io/fmt_code.h"
#include "syntax/lexer.h"
#include "syntax/token_input.h"
#include "syntax/combinator.h"

int main(void) {
	file_hnd file("C:/TMP/YoakkeTest/tokenizer.txt");
	if (!file.good()) {
		std::cout << "Could not open file: " << file.path() << '!' << std::endl;
	}
	lexer lex(file);
	token_input in(lex);
	
	//auto expr_m = combinator::terminal<token_t::IntLit>();
	//auto op_m = combinator::either(
	//	combinator::terminal<token_t::Add>(),
	//	combinator::terminal<token_t::Sub>()
	//);
	//
	//auto matcher = combinator::sequence(
	//	expr_m, op_m, expr_m
	//);
	//if (auto res = matcher(in)) {
	//	std::cout << "Match!" << std::endl;
	//}
	//else {
	//	std::cout << "No macth!" << std::endl;
	//}

	struct simple_op {
		token left;
		token op;
		token right;

		simple_op(token a, token b, token c) {
			left = a;
			op = b;
			right = c;
		}

		int eval() {
			int res = 0;
			int a = std::atoi(left.Value.c_str());
			int b = std::atoi(right.Value.c_str());
			switch (op.Type) {
			case token_t::Add: return a + b;
			case token_t::Sub: return a - b;
			}
			std::cout << "error" << std::endl;
			return 0;
		}
	};

	auto make_op = [](ytup<token, token, token>& tokens) {
		return simple_op(std::get<0>(tokens), std::get<1>(tokens), std::get<2>(tokens));
	};

	auto IntLit = combinator::terminal<token_t::IntLit>();
	auto Add = combinator::terminal<token_t::Add>();
	auto Sub = combinator::terminal<token_t::Sub>();

	auto BinOp = [=](auto res) {
		return combinator::wrap(make_op, res);
	};

	auto matcher =
		IntLit >= (*((Add | Sub) >= IntLit))
		;
	auto res = matcher(in);
	if (res.is_ok()) {
		auto& ok = res.get_ok();
		std::cout << typeid(ok).name() << std::endl;
	}
	else {
		std::cout << "No match!" << std::endl;
	}

	std::cin.get();
	return 0;
}
