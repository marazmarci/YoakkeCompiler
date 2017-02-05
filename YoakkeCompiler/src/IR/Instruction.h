#pragma once

#include "Opcode.h"

namespace yk
{
	namespace IR
	{
		class BasicBlock;

		class Instr
		{
		public:
			Opcode Code;

		public:
			Instr(Opcode c);
		};

		class JmpInstr : public Instr
		{
		public:
			BasicBlock* Target;

		public:
			JmpInstr(BasicBlock* targ);
		};
	}
}