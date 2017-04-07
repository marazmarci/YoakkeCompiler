#pragma once

#include "../common.h"
#include "scope.h"

namespace yk {
	class symbol;

	class symbol_table {
	private:
		yshared_ptr<scope> m_Global;
		yshared_ptr<scope> m_Current;

	public:
		symbol_table();

	public:
		void push(yshared_ptr<scope>& sc);
		void pop();
		sym_set const* ref(ystr const& ident);
		void decl(yshared_ptr<symbol>& sym);
	};
}
