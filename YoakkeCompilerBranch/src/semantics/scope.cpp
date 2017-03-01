#include "scope.h"
#include "symbol.h"

namespace yk {
	sym_set* scope::ref(ystr const& id) {
		auto it = Dict.find(id);
		if (it == Dict.end()) {
			if (Parent) {
				return Parent->ref(id);
			}
			else {
				return nullptr;
			}
		}
		else {
			return &it->second;
		}
	}

	void scope::decl(symbol* sym) {
		auto it = Dict.find(sym->identifier);
		if (it == Dict.end()) {
			sym_set set{ sym };
			Dict.insert(std::make_pair(sym->identifier, set));
		}
		else {
			it->second.push_back(sym);
		}
	}
}