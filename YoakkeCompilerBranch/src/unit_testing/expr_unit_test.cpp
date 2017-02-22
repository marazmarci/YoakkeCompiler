#include "expr_unit_test.h"
#include "../debug/unit_test.h"
#include "../parse/ylexer.h"
#include "../parse/yexpr_parser.h"
#include "../debug/expr_prec_printer.h"

#define PARSE(x) lexer.set_source(x);							\
				buffer.clear();									\
				exp = parser.parse();							\
				result = exp ? printer.dispatch_gen(exp) : ""
#define parse_expect(x, y) PARSE(x); UNIT_TEST_EXPECT(result == y, x == y)
#define parse_expect_not(x, y) PARSE(x); UNIT_TEST_EXPECT_NOT(result == y, x != y)
namespace yk {
	namespace unit_testing {
		void test_expr_parser() {
			yk::ylexer lexer;
			yk::token_buffer buffer(&lexer);
			yk::yexpr_parser parser(&buffer);
			yk::expr* exp = nullptr;
			ystr result;
			yk::expr_prec_printer printer;

			UNIT_TEST_MOD("Expression Parsing")
				UNIT_TEST_FUNC("Atomic")
					parse_expect("", "");
					parse_expect("a", "a");
				UNIT_TEST_ENDFUNC()

				UNIT_TEST_FUNC("Precedence and Associativity")
					parse_expect("a + b", "(a + b)");
					parse_expect("a + b + c", "((a + b) + c)");
					parse_expect_not("a + b + c", "(a + (b + c))");
					parse_expect("a = b = c", "(a = (b = c))");
					parse_expect_not("a = b = c", "((a = b) = c)");
				UNIT_TEST_ENDFUNC()
			UNIT_TEST_ENDMOD()
		}
	}
}
