#pragma once

#include "../common.h"
#include "../lexing/position.h"

namespace yk {
	class symbol;
	class type_symbol;
	using sym_set = yvec<ysptr<symbol>>;

	class scope {
	private:
		ysptr<scope> m_Parent;
		yopt_map<ystr, sym_set> m_Symbols;
		bool m_ReturnDest;
		ysptr<type_symbol> m_ReturnType;
		interval m_ReturnTypePos;

	public:
		scope();

	public:
		ysptr<scope> get_parent();
		void set_parent(ysptr<scope>& ptr);

		yopt<sym_set*> ref(ystr const& key);
		void decl(ysptr<symbol>& sym);

		void mark_return_dest();

		ysptr<type_symbol> get_return_type();
		void set_return_type(ysptr<type_symbol> rt, interval const& pos);
		interval const& get_return_type_pos();

		scope* get_enclosing_return_dest();
	};
}
