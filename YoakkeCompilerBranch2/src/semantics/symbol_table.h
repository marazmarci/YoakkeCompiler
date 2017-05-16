#pragma once

#include "../common.h"

namespace yk {
	struct type;
	struct var_sym;
	class scope;

	class symbol_table {
	public:
		static ysptr<type> UNIT_T;
		static ysptr<type> I32_T;
		static ysptr<type> F32_T;

	private:
		ysptr<scope> m_GlobalScope;
		ysptr<scope> m_CurrentScope;

	public:
		symbol_table();

	public:
		void decl(ysptr<var_sym> v);
		ysptr<var_sym> ref(ystr const& id);
		ysptr<var_sym> ref(ystr const& id, ysptr<type> hint);

		void push();
		void pop();
	};
}
