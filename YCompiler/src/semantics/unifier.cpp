#include <cassert>
#include "type.h"
#include "unifier.h"

namespace unifier {
	ystr to_str(type* ty) {
		switch (ty->Ty) {
		case type_t::Variable: {
			type_var* tv = (type_var*)ty;
			if (tv->Instance) {
				return to_str(tv->Instance);
			}
			return ystr{ '\'', char('A' + tv->ID % 26) };
		}

		case type_t::Constructor: {
			type_cons* tc = (type_cons*)ty;
			ystr res = tc->Name;
			if (tc->Params.size()) {
				res += '<' + to_str(tc->Params[0]);
				for (ysize i = 1; i < tc->Params.size(); i++) {
					res += ", " + to_str(tc->Params[i]);
				}
				res += '>';
			}
			return res;
		}

		case type_t::Set: {
			type_set* ts = (type_set*)ty;
			ystr res = "{";
			if (ts->Types.size()) {
				res += to_str(ts->Types[0]);
				for (ysize i = 1; i < ts->Types.size(); i++) {
					res += ", " + to_str(ts->Types[i]);
				}
			}
			res += '}';
			return res;
		}
		}
	}

	type* prune(type* ty) {
		switch (ty->Ty) {
		case type_t::Variable: {
			type_var* tv = (type_var*)ty;
			if (tv->Instance) {
				tv->Instance = prune(tv->Instance);
				return tv->Instance;
			}
			return tv;
		}

		case type_t::Set:
		case type_t::Constructor: {
			return ty;
		}
		}
	}

	bool contains(type* container, type_var* what) {
		type* ty = prune(container);

		switch (ty->Ty) {
		case type_t::Variable: {
			type_var* tv = (type_var*)ty;
			return tv->ID == what->ID;
		}

		case type_t::Constructor: {
			type_cons* tc = (type_cons*)ty;
			for (type* p : tc->Params) {
				if (contains(p, what)) {
					return true;
				}
			}
			return false;
		}

		case type_t::Set: {
			type_set* ts = (type_set*)ty;
			for (type* p : ts->Types) {
				if (contains(p, what)) {
					return true;
				}
			}
			return false;
		}
		}

		UNREACHABLE;
	}

	bool same(type* a, type* b) {
		type* t1 = prune(a);
		type* t2 = prune(b);

		if (   t1->Ty == type_t::Variable
			&& t2->Ty == type_t::Variable) {
			type_var* tt1 = (type_var*)t1;
			type_var* tt2 = (type_var*)t2;

			return tt1->ID == tt2->ID;
		}
		else if (t1->Ty == type_t::Constructor
			  && t2->Ty == type_t::Constructor) {
			type_cons* tt1 = (type_cons*)t1;
			type_cons* tt2 = (type_cons*)t2;

			if (tt1->Name != tt2->Name) {
				return false;
			}
			if (tt1->Params.size() != tt2->Params.size()) {
				return false;
			}
			for (ysize i = 0; i < tt1->Params.size(); i++) {
				if (!same(tt1->Params[i], tt2->Params[i])) {
					return false;
				}
			}
			return true;
		}
		else if (t1->Ty == type_t::Set
			  && t2->Ty == type_t::Set) {
			UNIMPLEMENTED;
		}
		else {
			return false;
		}

		UNREACHABLE;
	}

	namespace {
		bool match_var_var(type_var* tv1, type_var* tv2) {
			return true;
		}

		bool match_var_cons(type_var* tt1, type_cons* tt2) {
			if (contains(tt2, tt1)) {
				return false;
			}
			return true;
		}

		bool match_cons_cons(type_cons* tt1, type_cons* tt2) {
			if (tt1->Name != tt2->Name) {
				return false;
			}
			if (tt1->Params.size() != tt2->Params.size()) {
				return false;
			}
			for (ysize i = 0; i < tt1->Params.size(); i++) {
				if (!match(tt1->Params[i], tt2->Params[i])) {
					return false;
				}
			}
			return true;
		}

		bool match_var_set(type_var* tt1, type_set* tt2) {
			if (contains(tt2, tt1)) {
				return false;
			}
			return true;
		}

		bool match_cons_set(type_cons* tt1, type_set* tt2) {
			bool matched = true;
			for (auto& t : tt2->Types) {
				if (match(tt1, t)) {
					return true;
				}
			}
			return false;
		}

		bool match_set_set(type_set* tt1, type_set* tt2) {
			UNIMPLEMENTED;
		}
	}

	bool match(type* a, type* b) {
		type* t1 = prune(a);
		type* t2 = prune(b);

		if (t1->Ty == type_t::Variable) {
			type_var* tt1 = (type_var*)t1;

			if (t2->Ty == type_t::Variable) {
				type_var* tt2 = (type_var*)t2;
				return match_var_var(tt1, tt2);
			}
			else if (t2->Ty == type_t::Constructor) {
				type_cons* tt2 = (type_cons*)t2;
				return match_var_cons(tt1, tt2);
			}
			else if (t2->Ty == type_t::Set) {
				type_set* tt2 = (type_set*)t2;
				return match_var_set(tt1, tt2);
			}
			else {
				UNREACHABLE;
			}
		}
		else if (t1->Ty == type_t::Constructor) {
			type_cons* tt1 = (type_cons*)t1;

			if (t2->Ty == type_t::Variable) {
				type_var* tt2 = (type_var*)t2;
				return match_var_cons(tt2, tt1);
			}
			else if (t2->Ty == type_t::Constructor) {
				type_cons* tt2 = (type_cons*)t2;
				return match_cons_cons(tt1, tt2);
			}
			else if (t2->Ty == type_t::Set) {
				type_set* tt2 = (type_set*)t2;
				return match_cons_set(tt1, tt2);
			}
			else {
				UNREACHABLE;
			}
		}
		else if (t1->Ty == type_t::Set) {
			type_set* tt1 = (type_set*)t1;

			if (t2->Ty == type_t::Variable) {
				type_var* tt2 = (type_var*)t2;
				return match_var_set(tt2, tt1);
			}
			else if (t2->Ty == type_t::Constructor) {
				type_cons* tt2 = (type_cons*)t2;
				return match_cons_set(tt2, tt1);
			}
			else if (t2->Ty == type_t::Set) {
				type_set* tt2 = (type_set*)t2;
				return match_set_set(tt1, tt2);
			}
			else {
				UNREACHABLE;
			}
		}
		else {
			UNREACHABLE;
		}

		UNREACHABLE;
	}

	namespace {
		yopt<ystr> unify_var_var(type_var* tv1, type_var* tv2) {
			if (tv1->ID != tv2->ID) {
				tv1->Instance = tv2;
			}
			return {};
		}

		yopt<ystr> unify_var_cons(type_var* tt1, type_cons* tt2) {
			if (contains(tt2, tt1)) {
				return ystr("Recursive type: " + to_str(tt2) + " contains " + to_str(tt1));
			}
			tt1->Instance = tt2;
			return {};
		}

		yopt<ystr> unify_cons_cons(type_cons* tt1, type_cons* tt2) {
			if (tt1->Name != tt2->Name) {
				return ystr(to_str(tt1) + " is not " + to_str(tt2));
			}
			if (tt1->Params.size() != tt2->Params.size()) {
				return ystr(to_str(tt1) + " has different number of type parameters than " + to_str(tt2));
			}
			for (ysize i = 0; i < tt1->Params.size(); i++) {
				if (auto err = unify(tt1->Params[i], tt2->Params[i])) {
					return *err;
				}
			}
			return {};
		}

		yopt<ystr> unify_var_set(type_var* tt1, type_set* tt2) {
			if (contains(tt2, tt1)) {
				return ystr("Recursive type: " + to_str(tt2) + " contains " + to_str(tt1));
			}
			tt1->Instance = tt2;
			return {};
		}

		yopt<ystr> unify_cons_set(type_cons* tt1, type_set* tt2) {
			yopt<type*> matched = {};
			for (auto& t : tt2->Types) {
				if (match(tt1, t)) {
					if (matched) {
						return ystr("Multiple overload found for " + to_str(tt1) + " in " + to_str(tt2));
					}
					matched = t;
				}
			}
			if (!matched) {
				return ystr("No overload found for " + to_str(tt1) + " in " + to_str(tt2));
			}
			return unify(tt1, *matched);
		}

		yopt<ystr> unify_set_set(type_set* tt1, type_set* tt2) {
			UNIMPLEMENTED;
		}
	}

	yopt<ystr> unify(type* a, type* b) {
		type* t1 = prune(a);
		type* t2 = prune(b);

		if (t1->Ty == type_t::Variable) {
			type_var* tt1 = (type_var*)t1;

			if (t2->Ty == type_t::Variable) {
				type_var* tt2 = (type_var*)t2;
				return unify_var_var(tt1, tt2);
			}
			else if (t2->Ty == type_t::Constructor) {
				type_cons* tt2 = (type_cons*)t2;
				return unify_var_cons(tt1, tt2);
			}
			else if (t2->Ty == type_t::Set) {
				type_set* tt2 = (type_set*)t2;
				return unify_var_set(tt1, tt2);
			}
			else {
				UNREACHABLE;
			}
		}
		else if (t1->Ty == type_t::Constructor) {
			type_cons* tt1 = (type_cons*)t1;

			if (t2->Ty == type_t::Variable) {
				type_var* tt2 = (type_var*)t2;
				return unify_var_cons(tt2, tt1);
			}
			else if (t2->Ty == type_t::Constructor) {
				type_cons* tt2 = (type_cons*)t2;
				return unify_cons_cons(tt1, tt2);
			}
			else if (t2->Ty == type_t::Set) {
				type_set* tt2 = (type_set*)t2;
				return unify_cons_set(tt1, tt2);
			}
			else {
				UNREACHABLE;
			}
		}
		else if (t1->Ty == type_t::Set) {
			type_set* tt1 = (type_set*)t1;

			if (t2->Ty == type_t::Variable) {
				type_var* tt2 = (type_var*)t2;
				return unify_var_set(tt2, tt1);
			}
			else if (t2->Ty == type_t::Constructor) {
				type_cons* tt2 = (type_cons*)t2;
				return unify_cons_set(tt2, tt1);
			}
			else if (t2->Ty == type_t::Set) {
				type_set* tt2 = (type_set*)t2;
				return unify_set_set(tt1, tt2);
			}
			else {
				UNREACHABLE;
			}
		}
		else {
			UNREACHABLE;
		}

		UNREACHABLE;
	}
}
