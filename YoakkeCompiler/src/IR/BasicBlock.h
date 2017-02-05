#pragma once

#include "../Types.h"

namespace yk
{
	namespace IR
	{
		class Function;

		class BasicBlock
		{
		public:
			ystr Name;
			Function* Func;

		public:
			BasicBlock(ystr const& name);
		};
	}
}