#include "llvm_ir.h"
#include "../ir/ir_module.h"
#include "../ir/ir_function.h"
#include "../ir/ir_basic_block.h"
#include "../ir/ir_instr.h"
#include "../ir/ir_type.h"
#include "../ir/ir_value.h"

namespace yk {
	llvm_ir::llvm_ir(std::ostream& os)
		: m_Ostream(os) {
	}

	void llvm_ir::print(ir_module* mod) {
		for (auto f : mod->Functions) {
			print(f);
			m_Ostream << std::endl;
		}
	}

	void llvm_ir::print(ir_function* fn) {
		auto proto = fn->Prototype;
		m_Ostream << "define " << proto->ReturnType->Identifier << " @" << proto->Name << "(";
		if (proto->Parameters.size()) {
			m_Ostream << proto->Parameters[0]->Type->Identifier << ' '
				<< proto->Parameters[0]->Name;
			for (ysize i = 1; i < proto->Parameters.size(); i++) {
				m_Ostream << ", "
					<< proto->Parameters[i]->Type->Identifier << ' '
					<< proto->Parameters[i]->Name;
			}
		}
		m_Ostream << ") {" << std::endl;
		for (auto bb : fn->Blocks) {
			print(bb);
		}
		m_Ostream << "}" << std::endl;
	}

	void llvm_ir::print(ir_basic_block* bb) {
		m_Ostream << bb->Name << ':' << std::endl;
		for (auto i : bb->Instructions) {
			m_Ostream << "  ";
			print(i);
			m_Ostream << std::endl;
		}
	}

	void llvm_ir::print(ir_instr* ins) {
		switch (ins->Opcode) {
		/*case ir_opcode::jmp: {
			auto i2 = reinterpret_cast<ir_jmp_instr*>(ins);
			m_Ostream << "jmp label<" << i2->Destination->Name << '>';
			break;
		}*/

		case ir_opcode::ret: {
			auto i2 = reinterpret_cast<ir_ret_instr*>(ins);
			m_Ostream << "ret ";
			if (i2->Value) {
				m_Ostream << i2->Value->Type->Identifier << ' ';
				print(i2->Value);
			}
			else {
				m_Ostream << "void";
			}
			break;
		}

		case ir_opcode::alloc: {
			auto i2 = reinterpret_cast<ir_alloc_instr*>(ins);
			m_Ostream << '%' << i2->Name << " = alloca " << i2->What->Identifier;
			break;
		}

		case ir_opcode::store: {
			auto i2 = reinterpret_cast<ir_store_instr*>(ins);
			m_Ostream << "store " << i2->Value->Type->Identifier << ' ';
			print(i2->Value);
			m_Ostream << ", " << i2->Ptr->Type->Identifier << ' ';
			print(i2->Ptr);
			break;
		}

		default:
			throw std::exception("Uncovered opcode for IR printer!");
		}
	}

	void llvm_ir::print(ir_value* val) {
		if (auto i_t = dynamic_cast<ir_int_value*>(val)) {
			m_Ostream << i_t->Value;
		}
		else if (auto n_t = dynamic_cast<ir_named_value*>(val)) {
			m_Ostream << '%' << n_t->Name;
		}
		else {
			throw std::exception("Unsopported value for IR printer!");
		}
	}
}