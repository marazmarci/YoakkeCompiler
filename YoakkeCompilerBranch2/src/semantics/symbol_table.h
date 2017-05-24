#pragma once

#include "../common.h"

namespace yk {
	struct type;
	struct symbol;
	class scope;
	class ty_scope;

	class symbol_table {
	public:
		static type UNIT_T;
		static type I32_T;
		static type F32_T;

	private:
		ysptr<scope>	m_GlobalScope;
		ysptr<scope>	m_CurrentScope;
		ysptr<ty_scope> m_GlobalTScope;
		ysptr<ty_scope> m_CurrentTScope;

	public:
		symbol_table();

	public:
		void decl(symbol const& v);
		void decl(ystr const& id, type& t);
		yopt<yref<symbol>> ref(ystr const& id);
		yopt<yref<type>> ref_t(ystr const& id);
		scope* enclosing_return();

		ysptr<scope> push(bool ret_dest = false);
		void pop();
	};
}
