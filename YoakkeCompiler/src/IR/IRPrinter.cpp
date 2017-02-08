#include "IRPrinter.h"
#include "Unit.h"
#include "Function.h"
#include "BasicBlock.h"
#include "Type.h"
#include "Instruction.h"

namespace yk
{
	namespace IR
	{
		IRPrinter::IRPrinter(std::ostream& os)
			: m_Ostream(os)
		{
		}

		void IRPrinter::Print(Unit* un)
		{
			for (auto e : un->Functions)
				Print(e);
		}

		void IRPrinter::Print(Function* fn)
		{
			m_Ostream << "function " << fn->ReturnType->Identifier
				<< " @" << fn->Name << '(';
			if (fn->Parameters.size())
			{
				auto& par = fn->Parameters[0];
				m_Ostream << par.PType->Identifier << ' ' << par.Name;
				for (ysize i = 1; i < fn->Parameters.size(); i++)
				{
					par = fn->Parameters[i];
					m_Ostream << ", " << par.PType->Identifier << ' ' << par.Name;
				}
			}
			m_Ostream << ") {" << std::endl;
			for (auto e : fn->Blocks)
				Print(e);
			m_Ostream << '}' << std::endl;
		}

		void IRPrinter::Print(BasicBlock* bb)
		{
			m_Ostream << "<label> '" << bb->Name << "':" << std::endl;
			for (auto i : bb->Body)
				Print(i);
		}

		void IRPrinter::Print(Instr* ins)
		{
			m_Ostream << "  ";
			switch (ins->Code)
			{
			case Opcode::Jmp:
				m_Ostream << "jmp label '" << ((JmpInstr*)ins)->Target->Name << '\'';
				break;

			default:
				std::cout << "Unhandled opcode!" << std::endl;
			}
			m_Ostream << std::endl;
		}
	}
}