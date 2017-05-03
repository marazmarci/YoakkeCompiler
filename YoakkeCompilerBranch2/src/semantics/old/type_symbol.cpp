#include "type_symbol.h"

namespace yk {
	type_symbol::type_symbol(ystr const& id)
		: symbol(id) {
	}

	type_symbol::~type_symbol() { }

	bool type_symbol::same(ysptr<type_symbol> other) {
		return match(other);
	}

	bool type_symbol::match(ysptr<type_symbol> other) {
		return Identifier == other->Identifier;
	}

	// Native type symbol

	native_type_symbol::native_type_symbol(ystr const& id)
		: type_symbol(id) {
	}

	native_type_symbol::~native_type_symbol() { }

	// Function type symbol

	ystr fn_type_symbol::create_name(ysptr<type_symbol> p, ysptr<type_symbol> r) {
		return "((" + p->Identifier + ")->(" + r->Identifier + "))";
	}

	fn_type_symbol::fn_type_symbol(ysptr<type_symbol> p, ysptr<type_symbol> r)
		: type_symbol(create_name(p, r)), Parameters(p), Return(r) {
	}
	
	fn_type_symbol::~fn_type_symbol() { }

	bool fn_type_symbol::same(ysptr<type_symbol> other) {
		if (match(other)) {
			auto o2 = std::dynamic_pointer_cast<fn_type_symbol>(other);
			return Return->same(o2->Return);
		}
		return false;
	}
	
	bool fn_type_symbol::match(ysptr<type_symbol> other) {
		// Just check parameters
		if (auto o2 = std::dynamic_pointer_cast<fn_type_symbol>(other)) {
			return Parameters->same(o2->Parameters);
		}
		return false;
	}

	// Tuple type symbol

	ystr tuple_type_symbol::create_name(yvec<ysptr<type_symbol>> const& ls) {
		ystr res = "(";
		if (ls.size()) {
			res += ls[0]->Identifier;
			for (std::size_t i = 1; i < ls.size(); i++) {
				res += "," + ls[i]->Identifier;
			}
		}
		res += ')';
		return res;
	}

	tuple_type_symbol::tuple_type_symbol(yvec<ysptr<type_symbol>>& ls)
		: type_symbol(create_name(ls)), Elements(ls) {
	}

	tuple_type_symbol::~tuple_type_symbol() { }

	bool tuple_type_symbol::same(ysptr<type_symbol> other) {
		return match(other);
	}

	bool tuple_type_symbol::match(ysptr<type_symbol> other) {
		if (auto o2 = std::dynamic_pointer_cast<tuple_type_symbol>(other)) {
			if (Elements.size() == o2->Elements.size()) {
				for (std::size_t i = 0; i < Elements.size(); i++) {
					if (!Elements[i]->same(o2->Elements[i])) {
						return false;
					}
				}
				return true;
			}
		}
		return false;
	}

	// Unknown type symbol

	unknown_type_symbol::unknown_type_symbol(ysptr<typed_symbol> o) 
		: type_symbol("@unknown"), Owner(o) {
	}

	unknown_type_symbol::~unknown_type_symbol() { }

	bool unknown_type_symbol::same(ysptr<type_symbol> other) {
		return false;
	}

	bool unknown_type_symbol::match(ysptr<type_symbol> other) {
		return false;
	}
}
