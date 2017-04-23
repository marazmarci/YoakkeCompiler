#include "scope.h"
#include "symbol.h"

namespace yk {
	scope::scope()
		: m_Parent(nullptr), m_ReturnDest(false), m_ReturnType(nullptr) {
	}

	ysptr<scope> scope::get_parent() {
		return m_Parent;
	}

	void scope::set_parent(ysptr<scope>& ptr) {
		if (m_Parent) {
			throw std::exception("Cannot set parent twice for scope!");
		}
		m_Parent = ptr;
	}

	yopt<sym_set*> scope::ref(ystr const& key) {
		if (auto set_opt = m_Symbols.at(key)) {
			return &set_opt.value().get();
		}
		if (m_Parent) {
			return m_Parent->ref(key);
		}
		return {};
	}

	void scope::decl(ysptr<symbol>& sym) {
		if (auto set_opt = m_Symbols.at(sym->Identifier)) {
			set_opt->get().push_back(sym);
		}
		else {
			m_Symbols.insert(std::make_pair(sym->Identifier, sym_set{ sym }));
		}
	}

	void scope::mark_return_dest() {
		m_ReturnDest = true;
	}

	ysptr<type_symbol> scope::get_return_type() {
		return m_ReturnType;
	}

	void scope::set_return_type(ysptr<type_symbol> rt) {
		m_ReturnType = rt;
	}

	scope* scope::get_enclosing_return_dest() {
		if (m_ReturnDest) {
			return this;
		}
		if (m_Parent) {
			return m_Parent->get_enclosing_return_dest();
		}
		return nullptr;
	}
}
