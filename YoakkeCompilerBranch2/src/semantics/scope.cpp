#include "scope.h"
#include "symbol.h"

namespace yk {
	scope::scope()
		: m_Parent(nullptr) {
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
}
