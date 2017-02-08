#pragma once

#include "../Parsing/YExprParser.h"	

namespace yk
{
	namespace dbg
	{
		void Parser_TestAll();
		bool Parser_Test(ystr const& src, ystr const& expect, bool flip);
	}
}