#include "symbol_table.h"
#include "scope.h"
#include "type_symbol.h"

namespace yk {
	symbol_table::symbol_table() {
		decl(new builtin_type_symbol("i32"));
		decl(new builtin_type_symbol("unit"));
	}

	bool symbol_table::is_global() {
		return m_GlobalScope == m_CurrentScope;
	}

	void symbol_table::push(scope* sc) {
		m_CurrentScope->Children.push_back(sc);
		sc->Parent = m_CurrentScope;
		m_CurrentScope = sc;
	}

	void symbol_table::pop(scope* sc) {
		if (m_CurrentScope == m_GlobalScope) {
			throw std::exception("Cannot pop global scope!");
		}
		else {
			m_CurrentScope = m_CurrentScope->Parent;
		}
	}

	void symbol_table::decl(symbol* sym) {
		m_CurrentScope->decl(sym);
	}

	sym_set* symbol_table::ref(ystr const& id) {
		return m_CurrentScope->ref(id);
	}
}