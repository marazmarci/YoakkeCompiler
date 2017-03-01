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
		for (auto p : mod->FunctionDecls) {
			print(p);
			m_Ostream << std::endl;
		}
		for (auto f : mod->Functions) {
			print(f);
			m_Ostream << std::endl;
		}
	}

	void llvm_ir::print(ir_function* fn) {
		auto proto = fn->Prototype;
		m_Ostream << "define " << proto->ReturnType->identifier << " @" << proto->Name << "(";
		if (proto->Parameters.size()) {
			m_Ostream << proto->Parameters[0]->Type->identifier << ' '
				<< '%' << proto->Parameters[0]->Name;
			for (ysize i = 1; i < proto->Parameters.size(); i++) {
				m_Ostream << ", "
					<< proto->Parameters[i]->Type->identifier << ' '
					<< '%' << proto->Parameters[i]->Name;
			}
		}
		m_Ostream << ") {" << std::endl;
		for (auto bb : fn->Blocks) {
			print(bb);
		}
		m_Ostream << "}" << std::endl;
	}

	void llvm_ir::print(ir_function_proto* pr) {
		m_Ostream << "declare " << pr->ReturnType->identifier << " @" << pr->Name << "(";
		if (pr->Parameters.size()) {
			m_Ostream << pr->Parameters[0]->Type->identifier;
			for (ysize i = 1; i < pr->Parameters.size(); i++) {
				m_Ostream << ", " << pr->Parameters[i]->Type->identifier;
			}
		}
		m_Ostream << ')' << std::endl;
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
				m_Ostream << i2->Value->Type->identifier << ' ';
				print(i2->Value);
			}
			else {
				m_Ostream << "void";
			}
			break;
		}

		case ir_opcode::alloc: {
			auto i2 = reinterpret_cast<ir_alloc_instr*>(ins);
			m_Ostream << '%' << i2->Name << " = alloca " << i2->What->identifier;
			break;
		}

		case ir_opcode::store: {
			auto i2 = reinterpret_cast<ir_store_instr*>(ins);
			m_Ostream << "store " << i2->Value->Type->identifier << ' ';
			print(i2->Value);
			m_Ostream << ", " << i2->Ptr->Type->identifier << ' ';
			print(i2->Ptr);
			break;
		}

		case ir_opcode::load: {
			auto i2 = reinterpret_cast<ir_load_instr*>(ins);
			m_Ostream << '%' << i2->Name << " = load " << i2->Type->identifier << ", "
				<< i2->Ptr->Type->identifier << ' ';
			print(i2->Ptr);
			break;
		}

		case ir_opcode::call: {
			auto i2 = reinterpret_cast<ir_call_instr*>(ins);
			if (i2->Type->identifier != "void") {
				m_Ostream << '%' << i2->Name << " = ";
			}
			m_Ostream << "call " << i2->FP->ReturnType->identifier << " @" << i2->FP->Name 
				<< '(';
			if (i2->Args.size()) {
				m_Ostream << i2->Args[0]->Type->identifier << ' ';
				print(i2->Args[0]);
				for (ysize i = 1; i < i2->Args.size(); i++) {
					m_Ostream << ", " << i2->Args[i]->Type->identifier << ' ';
					print(i2->Args[i]);
				}
			}
			m_Ostream << ')';
			break;
		}

		case ir_opcode::iadd: {
			auto i2 = reinterpret_cast<ir_iadd_instr*>(ins);
			m_Ostream << '%' << i2->Name << " =  add i32 ";
			print(i2->Left);
			m_Ostream << ", ";
			print(i2->Right);
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