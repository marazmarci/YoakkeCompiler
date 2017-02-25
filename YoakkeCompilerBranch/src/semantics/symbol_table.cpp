#include "symbol_table.h"
#include "scope.h"
#include "type_symbol.h"

namespace yk {
	builtin_type_symbol* symbol_table::UNIT = nullptr;
	builtin_type_symbol* symbol_table::INT32 = nullptr;

	symbol_table::symbol_table() {
		decl(UNIT = new builtin_type_symbol("unit"));
		decl(INT32 = new builtin_type_symbol("i32"));
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