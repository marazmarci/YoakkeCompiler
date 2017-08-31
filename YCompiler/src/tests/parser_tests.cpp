#include "framework.h"
#include "parser_tests.h"
#include "../syntax/ast_expr.h"
#include "../io/file_hnd.h"
#include "../syntax/lexer.h"
#include "../syntax/parser.h"

namespace {
	ystr print_expr(AST_expr* exp) {
		switch (exp->Ty) {
		case AST_expr_t::Pre: {
			auto expr = (AST_pre_expr*)exp;
			return '(' + expr->Oper.Value + print_expr(expr->Subexpr) + ')';
		}

		case AST_expr_t::Bin: {
			auto expr = (AST_bin_expr*)exp;
			return '(' + print_expr(expr->Left) + expr->Oper.Value +
				print_expr(expr->Right) + ')';
		}

		case AST_expr_t::Post: {
			auto expr = (AST_post_expr*)exp;
			return '(' + print_expr(expr->Subexpr) + expr->Oper.Value + ')';
		}

		case AST_expr_t::IntLit: {
			auto expr = (AST_int_lit_expr*)exp;
			return std::to_string(expr->Value);
		}

		case AST_expr_t::RealLit: {
			auto expr = (AST_real_lit_expr*)exp;
			return std::to_string(expr->Value);
		}

		case AST_expr_t::BoolLit: {
			auto expr = (AST_bool_lit_expr*)exp;
			return expr->Value ? "true" : "false";
		}

		case AST_expr_t::Ident: {
			auto expr = (AST_ident_expr*)exp;
			return expr->Value;
		}

		default: UNIMPLEMENTED;
		}

		UNREACHABLE;
		return "";
	}

	ystr print_expr(const char* orig) {
		file_hnd f("<test>", orig);
		lexer lex(f);
		token_input in(lex);
		auto res = parser::parse_expr(in);
		if (res.is_ok()) {
			return print_expr(std::get<0>(res.get_ok()));
		}
		return ystr("<error>");
	}
}

void add_parser_tests() {
	TEST_CASE("primitives")
	{
		TEST_SUCC(print_expr("a") == "a");
		TEST_SUCC(print_expr("1") == "1");
		TEST_SUCC(print_expr("true") == "true");
		TEST_SUCC(print_expr("false") == "false");

		TEST_FAIL(print_expr("1") == "a");
		TEST_FAIL(print_expr("true") == "b");
		TEST_FAIL(print_expr("1") == "1.0");
	};

	TEST_CASE("precedence")
	{
		TEST_SUCC(print_expr("1+2") == "(1+2)");
		TEST_SUCC(print_expr("1+2*3") == "(1+(2*3))");
		TEST_SUCC(print_expr("1+2*3-4") == "((1+(2*3))-4)");
		TEST_SUCC(print_expr("1*(2+3)") == "(1*(2+3))");

		TEST_FAIL(print_expr("1+2*3") == "((1+2)*3)");
		TEST_FAIL(print_expr("(1+2)*3") == "(1+(2*3))");
	};

	TEST_CASE("associativity")
	{
		TEST_SUCC(print_expr("1+2+3") == "((1+2)+3)");
		TEST_SUCC(print_expr("1+(2+3)") == "(1+(2+3))");
		TEST_SUCC(print_expr("1*2*3") == "((1*2)*3)");
		TEST_SUCC(print_expr("a=b=c") == "(a=(b=c))");

		TEST_FAIL(print_expr("1+2+3") == "(1+(2+3))");
		TEST_FAIL(print_expr("a=b=c") == "((a=b)=c)");
	};
}
