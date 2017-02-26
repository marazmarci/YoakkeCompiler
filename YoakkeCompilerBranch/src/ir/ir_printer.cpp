#include "ir_printer.h"
#include "ir_module.h"
#include "ir_function.h"
#include "ir_basic_block.h"
#include "ir_instr.h"
#include "ir_type.h"

namespace yk {
	ir_printer::ir_printer(std::ostream& os)
		: m_Ostream(os) {
	}

	void ir_printer::print(ir_module* mod) {
		for (auto f : mod->Functions) {
			print(f);
			m_Ostream << std::endl;
		}
	}

	void ir_printer::print(ir_function* fn) {
		m_Ostream << "def " << fn->ReturnType->Identifier << " @" << fn->Name << "(";
		if (fn->Parameters.size()) {
			m_Ostream << fn->Parameters[0]->Type->Identifier << ' ' << fn->Parameters[0]->Name;
			for (ysize i = 1; i < fn->Parameters.size(); i++) {
				m_Ostream << ", " 
					<< fn->Parameters[i]->Type->Identifier << ' ' << fn->Parameters[i]->Name;
			}
		}
		m_Ostream << ") {" << std::endl;
		for (auto bb : fn->Blocks) {
			print(bb);
		}
		m_Ostream << "}" << std::endl;
	}

	void ir_printer::print(ir_basic_block* bb) {
		m_Ostream << "; label<" << bb->Name << ">:" << std::endl;
		for (auto i : bb->Instructions) {
			m_Ostream << "    ";
			print(i);
			m_Ostream << std::endl;
		}
	}

	void ir_printer::print(ir_instr* ins) {
		switch (ins->Opcode) {
		case ir_opcode::jmp: {
			auto i2 = reinterpret_cast<ir_jmp_instr*>(ins);
			m_Ostream << "jmp label<" << i2->Destination->Name << '>';
			break;
		}

		case ir_opcode::ret: {
			//auto i2 = reinterpret_cast<ir_jmp_instr*>(ins);
			m_Ostream << "ret";
			break;
		}

		default:
			throw std::exception("Uncovered opcode for IR printer!");
		}
	}
}