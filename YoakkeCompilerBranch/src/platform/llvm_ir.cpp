#include "llvm_ir.h"
#include "../ir/ir_module.h"
#include "../ir/ir_function.h"
#include "../ir/ir_basic_block.h"
#include "../ir/ir_instr.h"
#include "../ir/ir_type.h"
#include "../ir/ir_value.h"
#include "../environment/memory.h"

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
		m_Ostream << "define ";
		print(proto->ReturnType);
		m_Ostream << " @" << proto->Name << "(";
		if (proto->Parameters.size()) {
			print(proto->Parameters[0]->Type);
			m_Ostream << " %" << proto->Parameters[0]->Name;
			for (ysize i = 1; i < proto->Parameters.size(); i++) {
				m_Ostream << ", ";
				print(proto->Parameters[i]->Type);
				m_Ostream << " %" << proto->Parameters[i]->Name;
			}
		}
		m_Ostream << ") {" << std::endl;
		for (auto bb : fn->Blocks) {
			print(bb);
		}
		m_Ostream << "}" << std::endl;
	}

	void llvm_ir::print(ir_function_proto* pr) {
		m_Ostream << "declare ";
		print(pr->ReturnType);
		m_Ostream << " @" << pr->Name << "(";
		if (pr->Parameters.size()) {
			print(pr->Parameters[0]->Type);
			for (ysize i = 1; i < pr->Parameters.size(); i++) {
				m_Ostream << ", ";
				print(pr->Parameters[i]->Type);
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
				print(i2->Value->Type);
				m_Ostream << ' ';
				print(i2->Value);
			}
			else {
				m_Ostream << "void";
			}
			break;
		}

		case ir_opcode::alloc: {
			auto i2 = reinterpret_cast<ir_alloc_instr*>(ins);
			m_Ostream << '%' << i2->Name << " = alloca ";
			print(i2->What);
			break;
		}

		case ir_opcode::store: {
			auto i2 = reinterpret_cast<ir_store_instr*>(ins);
			m_Ostream << "store ";
			print(i2->Value->Type);
			m_Ostream << ' ';
			print(i2->Value);
			m_Ostream << ", ";
			print(i2->Ptr->Type);
			m_Ostream << ' ';
			print(i2->Ptr);
			break;
		}

		case ir_opcode::load: {
			auto i2 = reinterpret_cast<ir_load_instr*>(ins);
			m_Ostream << '%' << i2->Name << " = load ";
			print(i2->Type);
			m_Ostream << ", ";
			print(i2->Ptr->Type);
			m_Ostream << ' ';
			print(i2->Ptr);
			break;
		}

		case ir_opcode::call: {
			auto i2 = reinterpret_cast<ir_call_instr*>(ins);
			if (i2->Type->identifier != "void") {
				m_Ostream << '%' << i2->Name << " = ";
			}
			m_Ostream << "call ";
			print(i2->FP->ReturnType);
			m_Ostream<< " @" << i2->FP->Name << '(';
			if (i2->Args.size()) {
				print(i2->Args[0]->Type);
				m_Ostream << ' ';
				print(i2->Args[0]);
				for (ysize i = 1; i < i2->Args.size(); i++) {
					m_Ostream << ", ";
					print(i2->Args[i]->Type);
					m_Ostream << ' ';
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
		else if (auto r_t = dynamic_cast<ir_real_value*>(val)) {
			m_Ostream << "0x";
			auto val = r_t->Value;
			auto swapped = mem::swap_endian(r_t->Value);
			auto sarr = (ybyte*)&swapped;
			for (ysize i = 0; i < sizeof(swapped) * 2; i++) {
				// TODO: cut for float only
				if (i > 8) {
					m_Ostream << '0';
				}
				else {
					m_Ostream << "0123456789ABCDEF"[i % 2 ? 
							((sarr[i / 2] & 0x0f) >> 0)
						:	((sarr[i / 2] & 0xf0) >> 4)
					];
				}
			}
		}
		else if (auto n_t = dynamic_cast<ir_named_value*>(val)) {
			m_Ostream << '%' << n_t->Name;
		}
		else {
			throw std::exception("Unsopported value for IR printer!");
		}
	}

	void llvm_ir::print(ir_type* ty) {
		if (auto v_t = dynamic_cast<ir_void_type*>(ty)) {
			m_Ostream << "void";
		}
		else if (auto i_t = dynamic_cast<ir_int_type*>(ty)) {
			m_Ostream << i_t->identifier;
		}
		else if (auto r_t = dynamic_cast<ir_real_type*>(ty)) {
			if (r_t->Size == 32) {
				m_Ostream << "float";
			}
			else {
				m_Ostream << r_t->identifier;
			}
		}
		else if (auto p_t = dynamic_cast<ir_ptr_type*>(ty)) {
			print(p_t->Sub);
			m_Ostream << '*';
		}
		else {
			throw std::exception("Unsopported type for IR printer!");
		}
	}
}