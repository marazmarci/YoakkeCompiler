#include "symbol_table.h"
#include "scope.h"
#include "type.h"

namespace yk {
	ysptr<type> symbol_table::UNIT_T	= type_cons::create("unit");
	ysptr<type> symbol_table::I32_T		= type_cons::create("i32");
	ysptr<type> symbol_table::F32_T		= type_cons::create("f32");

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

	ysptr<var_sym> symbol_table::ref(ystr const& id, ysptr<type> hint) {
		return m_CurrentScope->ref(id, hint);
	}

	void symbol_table::push() {
		auto sc = std::make_shared<scope>();
		sc->Parent = m_CurrentScope;
		m_CurrentScope = sc;
	}

	void symbol_table::pop() {
		if (!m_CurrentScope->Parent) {
			throw std::exception("Tried to pop global scope!");
		}
		m_CurrentScope = m_CurrentScope->Parent;
	}
}
