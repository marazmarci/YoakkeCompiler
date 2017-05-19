#include "type.h"
#include "../utility/match.h"

namespace yk {
	// Type variable

	ysize type_var::s_IDcount = 0;

	type_var::type_var()
		: ID(s_IDcount++), Instance(nullptr) {
	}

	bool type_var::matches(ysptr<type> t) const {
		return true;
	}

	bool type_var::contains(ysptr<type_var> t) const {
		return ID == t->ID;
	}

	ystr type_var::to_str() const {
		if (Instance) {
			return Instance->to_str();
		}
		return ystr{ '\'', char('A' + ID % 26) };
	}

	// Type constructor

	type_cons::type_cons(ystr const& n, yvec<ysptr<type>> const& ts)
		: Name(n), Types(ts) {
	}

	bool type_cons::matches(ysptr<type> t) const {
		if (dyn_cast<type_var>(t)) {
			return true;
		}
		if (auto tc = dyn_cast<type_cons>(t)) {
			if (Name != tc->Name) {
				return false;
			}
			if (Types.size() != tc->Types.size()) {
				return false;
			}
			for (ysize i = 0; i < Types.size(); i++) {
				if (!Types[i]->matches(tc->Types[i])) {
					return false;
				}
			}
			return true;
		}
		throw std::exception("matches() sanity!");
	}

	bool type_cons::contains(ysptr<type_var> t) const {
		for (auto tt : Types) {
			if (tt->contains(t)) {
				return true;
			}
		}
		return false;
	}

	ystr type_cons::to_str() const {
		ystr res = Name;
		if (Types.size()) {
			res += '<' + Types[0]->to_str();
			for (ysize i = 1; i < Types.size(); i++) {
				res += ", " + Types[i]->to_str();
			}
			res += '>';
		}
		return res;
	}

	// Tuple type constructor

	tuple_type_cons::tuple_type_cons(yvec<ysptr<type>> const& ts)
		: type_cons("Tuple", ts) {
	}

	// Function type constructor

	fn_type_cons::fn_type_cons(yvec<ysptr<type>> const& ts)
		: type_cons("Function", ts) {
	}

	bool fn_type_cons::matches(ysptr<type> t) const {
		if (dyn_cast<type_var>(t)) {
			return true;
		}
		if (auto tc = dyn_cast<type_cons>(t)) {
			if (Name != tc->Name) {
				return false;
			}
			if (Types.size() != tc->Types.size()) {
				return false;
			}
			// TODO: assert Types.size() == 2
			for (ysize i = 0; i < Types.size() - 1; i++) {
				if (!Types[i]->matches(tc->Types[i])) {
					return false;
				}
			}
			return true;
		}
		throw std::exception("matches() sanity!");
	}
}
