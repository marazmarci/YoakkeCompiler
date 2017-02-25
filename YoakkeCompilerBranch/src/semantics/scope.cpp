#include "scope.h"
#include "symbol.h"

namespace yk {
	sym_set* scope::ref(ystr const& id) {
		auto it = Dict.find(id);
		if (it == Dict.end()) {
			return nullptr;
		}
		else {
			return &it->second;
		}
	}

	void scope::decl(symbol* sym) {
		auto it = Dict.find(sym->Identifier);
		if (it == Dict.end()) {
			sym_set set{ sym };
			Dict.insert(std::make_pair(sym->Identifier, set));
		}
		else {
			it->second.push_back(sym);
		}
	}
}