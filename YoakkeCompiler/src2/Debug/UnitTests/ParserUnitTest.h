#pragma once

#include <iostream>
#include "../UnitTest.h"

namespace yk
{
	namespace dbg
	{
		typedef UnitTest<ystr, ystr> ParserUnitTest;

		std::ostream& operator<<(std::ostream& os, ParserUnitTest::TestCase const& test);

		void Parser_UnitTest_All();
	}
}