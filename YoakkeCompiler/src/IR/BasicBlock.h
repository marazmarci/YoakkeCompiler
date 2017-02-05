#pragma once

#include "../Types.h"

namespace yk
{
	namespace IR
	{
		class Function;
		class Instr;

		class BasicBlock
		{
		public:
			ystr Name;
			Function* Func;
			yvec<Instr*> Body;

		public:
			BasicBlock(ystr const& name);

		public:
			void Add(Instr* ins);
		};
	}
}