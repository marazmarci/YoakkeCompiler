#include "scope.h"
#include "type.h"
#include "symbol.h"

namespace yk {
	scope::scope()
		: Parent(nullptr), ReturnDest(false), ReturnType(yopt<type>{}) {
	}

	yopt<yref<symbol>> scope::ref(ystr const& id) {
		auto it = Entries.find(id);
		if (it == Entries.end()) {
			if (Parent) {
				return Parent->ref(id);
			}
			else {
				return {};
			}
		}
		else {
			return it->second;
		}
	}

	void scope::decl(symbol const& var) {
		auto it = Entries.find(var.Name);
		if (it == Entries.end()) {
			Entries.insert({ var.Name, var });
		}
		else {
			// TODO: create a typeclass if constant or add to typeclass if already a typeclass
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

	// Type scope

	ty_scope::ty_scope()
		: Parent(nullptr) {
	}

	yopt<yref<type>> ty_scope::ref(ystr const& id) {
		auto it = Entries.find(id);
		if (it == Entries.end()) {
			if (Parent) {
				return Parent->ref(id);
			}
			else {
				return {};
			}
		}
		return it->second;
	}

	void ty_scope::decl(ystr const& id, type& ty) {
		if (ref(id)) {
			throw std::exception("TODO: type already defined");
		}
		Entries.insert({ id, ty });
	}
}
