#pragma once

#include "../common.h"

namespace yk {
	struct type;
	class var_sym;
	class scope;
	class ty_scope;

	class symbol_table {
	public:
		static type UNIT_T;
		static type I32_T;
		static type F32_T;

	private:
		ysptr<scope> m_GlobalScope;
		ysptr<scope> m_CurrentScope;
		ysptr<ty_scope> m_GlobalTScope;
		ysptr<ty_scope> m_CurrentTScope;

	public:
		symbol_table();

	public:
		void decl(ysptr<var_sym> v);
		void decl(ystr const& id, type& t);
		ysptr<var_sym> ref(ystr const& id);
		ypair<ysptr<var_sym>, bool> ref(ystr const& id, type& hint);
		type* ref_t(ystr const& id);
		scope* enclosing_return();

		ysptr<scope> push(bool ret_dest = false);
		void pop();
	};
}
