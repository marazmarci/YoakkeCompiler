#include "ir_builder.h"
#include "ir_module.h"
#include "ir_function.h"
#include "ir_basic_block.h"

namespace yk {
	ir_builder::ir_builder(ir_module* mod)
		: m_Mod(mod), m_CurrentFunc(nullptr), m_CurrentBB(nullptr) {
	}

	ir_function* ir_builder::current_func() {
		return m_CurrentFunc;
	}

	ir_basic_block* ir_builder::current_bb() {
		return m_CurrentBB;
	}

	void ir_builder::add_func(ir_function* func) {
		m_Mod->add(func);
		m_CurrentFunc = func;
	}

	void ir_builder::add_bb(ir_basic_block* bb) {
		m_CurrentFunc->add(bb);
		m_CurrentBB = bb;
	}

	void ir_builder::add_inst(ir_instr* inst) {
		m_CurrentBB->add(inst);
	}

	void ir_builder::add_inst_bb_begin(ir_instr* inst) {
		auto& blck = m_CurrentFunc->Blocks[0]->Instructions;
		if (blck.size()) {
			blck.insert(blck.begin(), inst);
		}
		else {
			blck.push_back(inst);
		}
	}

	void ir_builder::set_bb(ir_basic_block* bb) {
		m_CurrentBB = bb;
		m_CurrentFunc = bb->Function;
	}
}
