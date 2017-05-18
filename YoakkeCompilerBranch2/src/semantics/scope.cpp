#include "scope.h"
#include "type.h"
#include "var_sym.h"

namespace yk {
	scope::scope()
		: Parent(nullptr), ReturnDest(false), ReturnType(nullptr) {
	}

	ysptr<var_sym> scope::ref(ystr const& id) {
		auto it = Entries.find(id);
		if (it == Entries.end()) {
			if (Parent) {
				return Parent->ref(id);
			}
			else {
				return nullptr;
			}
		}
		else {
			auto& list = it->second;
			return *list.rbegin();
		}
	}

	ypair<ysptr<var_sym>, bool> scope::ref(ystr const& id, ysptr<type> hint) {
		auto it = Entries.find(id);
		if (it == Entries.end()) {
			if (Parent) {
				return Parent->ref(id, hint);
			}
			else {
				return { nullptr, false };
			}
		}
		else {
			auto& list = it->second;
			for (auto it2 = list.rbegin(); it2 != list.rend(); ++it2) {
				if ((*it2)->Type->matches(hint)) {
					return { *it2, it2 != list.rbegin() };
				}
			}
			if (Parent) {
				return Parent->ref(id, hint);
			}
			else {
				return { nullptr, true };
			}
		}
	}

	void scope::decl(ysptr<var_sym> var) {
		auto it = Entries.find(var->Name);
		if (it == Entries.end()) {
			yvec<ysptr<var_sym>> l;
			l.push_back(var);
			Entries.insert({ var->Name, l });
		}
		else {
			it->second.push_back(var);
		}
	}

	scope* scope::enclosing_return() {
		if (ReturnDest) {
			return this;
		}
		if (Parent) {
			return Parent->enclosing_return();
		}
		return nullptr;
	}
}
