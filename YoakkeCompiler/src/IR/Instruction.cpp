#include "Instruction.h"

namespace yk
{
	namespace IR
	{
		Instr::Instr(Opcode c)
			: Code(c)
		{
		}

		JmpInstr::JmpInstr(BasicBlock* targ)
			: Instr(Opcode::Jmp), Target(targ)
		{
		}
	}
}