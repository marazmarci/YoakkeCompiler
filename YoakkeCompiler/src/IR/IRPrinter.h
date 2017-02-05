#pragma once

#include <iostream>

namespace yk
{
	namespace IR
	{
		class Unit;
		class Function;
		class BasicBlock;
		class Instr;

		class IRPrinter
		{
		private:
			std::ostream& m_Ostream;

		public:
			IRPrinter(std::ostream& os);

		public:
			void Print(Unit* un);
			void Print(Function* fn);
			void Print(BasicBlock* bb);
			void Print(Instr* ins);
		};
	}
}