#include "ParserUnitTest.h"
#include "../../Parsing/YExprParser.h"

namespace yk
{
	namespace dbg
	{
		std::ostream& operator<<(std::ostream& os, ParserUnitTest::TestCase const& test)
		{
			os << "expected: '" << test.ExpectedResult << "', arguments: '"
				<< test.Arguments << '\'';
			return os;
		}

		void Parser_UnitTest_All()
		{
			auto tester = [](ystr const& arg) -> ystr {
				parse::Lexer lexer;
				yvec<parse::Token> tokbuf;
				Logger logger("ParserUnitTest");
				lexer.SetSource(arg.c_str(), "ParserUnitTest");
				auto parser = parse::YExprParser(&lexer, &tokbuf, &logger, "ParserUnitTest");
				auto exp = parser.ParseExpr();
				ystr res = "";
				if (exp)
				{
					res = exp->UnitTestPrint();
					delete exp;
				}
				return res;
			};

			ParserUnitTest test(tester, "ParserUnitTest");

			test.AddTest("", "", true);
			test.AddTest("a", "a", true);
			test.AddTest("a+b", "(a + b)", true);
			test.AddTest("a+b+c", "((a + b) + c)", true);
			test.AddTest("a-b+c", "((a - b) + c)", true);
			test.AddTest("a-b+c", "(a - (b + c))", false);
			test.AddTest("a+b-c", "((a + b) - c)", true);
			test.AddTest("a+b+c", "(a + (b + c))", false);
			test.AddTest("a+b+c+d", "(((a + b) + c) + d)", true);
			test.AddTest("a*b", "(a * b)", true);
			test.AddTest("a * b + c", "((a * b) + c)", true);
			test.AddTest("a * b + c", "(a * (b + c))", false);
			test.AddTest("a + b * c", "(a + (b * c))", true);
			test.AddTest("a + b * c", "((a + b) * c)", false);

			test.RunAll();
		}
	}
}