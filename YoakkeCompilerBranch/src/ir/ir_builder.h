#pragma once

namespace yk {
	class ir_module;
	class ir_function;
	class ir_basic_block;
	class ir_instr;

	class ir_builder {
	private:
		ir_module* m_Mod;
		ir_function* m_CurrentFunc;
		ir_basic_block* m_CurrentBB;

	public:
		ir_builder(ir_module* mod);

	public:
		ir_function* current_func();
		ir_basic_block* current_bb();
		void add_func(ir_function* func);
		void add_bb(ir_basic_block* bb);
		void add_inst(ir_instr* inst);
		void add_inst_bb_begin(ir_instr* inst);
		void set_bb(ir_basic_block* bb);
	};
}
