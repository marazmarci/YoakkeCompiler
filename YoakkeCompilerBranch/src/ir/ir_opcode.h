#pragma once

namespace yk {
	enum class ir_opcode {
		nop,
		jmp,
		alloc,
		store,
		load,
		call,
		iadd,
		ret
	};
}