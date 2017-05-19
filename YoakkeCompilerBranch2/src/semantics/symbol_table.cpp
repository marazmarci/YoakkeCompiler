#include "symbol_table.h"
#include "scope.h"
#include "type.h"
#include "var_sym.h"

namespace yk {
	type symbol_table::UNIT_T	= type::create_cons("unit");
	type symbol_table::I32_T	= type::create_cons("i32");
	type symbol_table::F32_T	= type::create_cons("f32");

	symbol_table::symbol_table() {
		m_GlobalScope = std::make_shared<scope>();
		m_CurrentScope = m_GlobalScope;
	}

	void symbol_table::decl(ysptr<var_sym> v) {
		m_CurrentScope->decl(v);
	}

	ysptr<var_sym> symbol_table::ref(ystr const& id) {
		return m_CurrentScope->ref(id);
	}

	ypair<ysptr<var_sym>, bool> symbol_table::ref(ystr const& id, type& hint) {
		return m_CurrentScope->ref(id, hint);
	}

	scope* symbol_table::enclosing_return() {
		return m_CurrentScope->enclosing_return();
	}

	ysptr<scope> symbol_table::push(bool ret_dest) {
		auto sc = std::make_shared<scope>();
		sc->ReturnDest = ret_dest;
		sc->Parent = m_CurrentScope;
		m_CurrentScope = sc;
		return sc;
	}

	void symbol_table::pop() {
		if (!m_CurrentScope->Parent) {
			throw std::exception("Tried to pop global scope!");
		}
		m_CurrentScope = m_CurrentScope->Parent;
	}
}
