#pragma once

#include "../common.h"

namespace yk {
	class symbol;
	using sym_set = yvec<ysptr<symbol>>;

	class scope {
	private:
		ysptr<scope> m_Parent;
		yopt_map<ystr, sym_set> m_Symbols;

	public:
		scope();

	public:
		ysptr<scope> get_parent();
		void set_parent(ysptr<scope>& ptr);

		yopt<sym_set*> ref(ystr const& key);
		void decl(ysptr<symbol>& sym);
	};
}
