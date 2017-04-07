#include "symbol_table.h"

#include "symbol.h"

namespace yk {
	symbol_table::symbol_table() {
		m_Global = std::make_shared<scope>();
		m_Current = m_Global;
	}

	void symbol_table::push(yshared_ptr<scope>& sc) {
		sc->set_parent(m_Current);
		m_Current = sc;
	}

	void symbol_table::pop() {
		if (auto par = m_Current->get_parent()) {
			m_Current = par;
		}
		else {
			throw std::exception("Cannot pop global scope!");
		}
	}

	sym_set const* symbol_table::ref(ystr const& ident) {
		if (auto set = m_Current->ref(ident)) {
			return set.value();
		}
		else {
			return nullptr;
		}
	}

	void symbol_table::decl(yshared_ptr<symbol>& sym) {
		m_Current->decl(sym);
	}
}
