#include <utility>
#include "ParserTest.h"
#include "Logger.h"

namespace yk
{
	namespace dbg
	{
		void Parser_TestAll()
		{
			typedef ytriplet<bool, ystr, ystr> test_elem;
			yvec<test_elem> tests =
			{
				test_elem{ true, "", "" },
				test_elem{ true, "a", "a" },
				test_elem{ true, "a + b", "(a + b)" },
				test_elem{ true, "a + b - c",	"((a + b) - c)"	},
				test_elem{ true, "a + b * c", "(a + (b * c))" },
				test_elem{ true, "(a)", "a" },
				test_elem{ true, "(a + b) * c)", "((a + b) * c)" },

				test_elem{ false, "a + b - c",	"(a + (b - c))" },
				test_elem{ false, "a + b * c", "((a + b) * c)" },
				test_elem{ false, "(a + b) * c)", "(a + (b * c))" },
			};

			ysize good = 0;
			for (auto& t : tests)
			{
				if (Parser_Test(std::get<1>(t), std::get<2>(t), std::get<0>(t)))
					good++;
			}
			//Logger::STATIC_INFO() << "Parser unit test ended, result: "
				//<< good << " / " << tests.size() << endlog;
		}

		bool Parser_Test(ystr const& src, ystr const& expect, bool flip)
		{
			parse::YExprParser parser(src.c_str(), "UNIT TEST");
			auto res = parser.ParseExpr();
			if (res)
			{
				auto res_str = res->UnitTestPrint();
				if (res_str == expect)
				{
					if (flip) return true;
					else
					{
						//Logger::STATIC_ERROR() << "Parser unit test succeeded for illegal case: '"
							//<< src << "'!" << endlog;
						return false;
					}
				}
				else
				{
					if (!flip) return true;
					else
					{
						//Logger::STATIC_ERROR() << "Parser unit test failed! Got: '"
							//<< src << "', but expected '" << expect << "'!" << endlog;
						return false;
					}
				}
			}
			else
			{
				if (expect == "") return true;
				if (flip)
				{
					//Logger::STATIC_INFO() << "Parser unit testing failer for: '"
						//<< src << "'!" << endlog;
					return false;
				}
				return true;
			}
		}
	}
}