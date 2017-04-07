#pragma once

#include "../common.h"

namespace yk {
	class symbol;
	using sym_set = yvec<yshared_ptr<symbol>>;

	class scope {
	private:
		yshared_ptr<scope> m_Parent;
		yopt_map<ystr, sym_set> m_Symbols;

	public:
		scope();

	public:
		yshared_ptr<scope> get_parent();
		void set_parent(yshared_ptr<scope>& ptr);

		yopt<sym_set*> ref(ystr const& key);
		void decl(yshared_ptr<symbol>& sym);
	};
}
