#include <cstdlib>
#include <iostream>
#include "io/file_hnd.h"
#include "io/fmt_code.h"
#include "syntax/lexer.h"
#include "syntax/token_input.h"
#include "syntax/combinator.h"
#include "functions.h"

struct expr {
	virtual float eval() = 0;
};

struct int_expr : public expr {
	int v;

	int_expr(token const& tok) 
		: v(std::atoi(tok.Value.c_str())) {
	}

	float eval() override {
		return v;
	}
};

struct float_expr : public expr {
	float v;

	float_expr(token const& tok)
		: v(std::atof(tok.Value.c_str())) {
	}

	float eval() override {
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

	float eval() override {
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
	
	auto IntLit = terminal<token_t::IntLit>("integer literal");
	auto RealLit = terminal<token_t::RealLit>("real literal");

	auto Add = terminal<token_t::Add>("'+'");
	auto Sub = terminal<token_t::Sub>("'-'");

	auto Mul = terminal<token_t::Mul>("'*'");
	auto Div = terminal<token_t::Div>("'/'");

	auto ComposeLeftAssoc = [](auto& left, auto& right_pair) -> expr* {
		auto& op = std::get<0>(right_pair);
		auto& right = std::get<1>(right_pair);
		return new bin_expr(left, op, right);
	};

	auto TransformLeftAssoc = [=](auto& left, auto& rights) -> expr* {
		return fnl::fold(rights.begin(), rights.end(), left, ComposeLeftAssoc);
	};

	auto Factor
		= ((IntLit) ^ [](token& tok) -> expr* { return new int_expr(tok); })
		| ((RealLit) ^ [](token& tok) -> expr* { return new float_expr(tok); });
	auto Term
		= (Factor >= (*((Mul | Div) >> Factor))) ^ TransformLeftAssoc;
	auto Expr
		= (Term >= (*((Add | Sub) >> Term))) ^ TransformLeftAssoc;

	auto res = Expr(in);
	if (res.is_ok()) {
		auto& res_ok = res.get_ok();
		auto& res_ninp = std::get<0>(res_ok);
		std::cout << res_ninp->eval() << std::endl;
	}
	else {
		// TODO: match
		auto& err = res.get_err().Error.Data;
		fnl::match(err)(
		[=](lexer_err& err) {
			fnl::match(err)(
			[=](lexer_eof_err& err) {
				std::cout << "EOF" << std::endl;
			},
			[=](lexer_unk_tok_err& err) {
				std::cout << "UNK TOK" << std::endl;
			}
			);
		},
		[=](parser_err& err) {
			fnl::match(err)(
			[=](parser_exp_tok_err& err) {
				auto& file = err.File;
				auto& token = err.Got;
				auto& pos = token.Pos;
				std::cout << "Syntax error in file: '"
					<< file.path() << "' at line "
					<< pos.Start.Row << ", character " << pos.Start.Column 
					<< '.' << std::endl;
				fmt_code::print(file, pos);
				std::cout << "Expected "
					<< err.Expected << ", but got "
					<< token.fmt() << '.' << std::endl;
			}
			);
		},
		[=](auto& mixture_error) {
			std::cout << "EITHER" << std::endl;
			for (auto i : mixture_error) {
				std::cout << "alt" << std::endl;
			}
		});
	}

	std::cin.get();
	return 0;
}
