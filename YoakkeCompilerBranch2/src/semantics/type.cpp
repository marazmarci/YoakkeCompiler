#include "type.h"
#include "../utility/match.h"

namespace yk {
	// Type variable

	ysize type::s_IDcount = 0;

	type type::create_var() {
		return type(std::make_shared<var_type>(s_IDcount++, yopt<type>{}));
	}

	type type::create_cons(ystr const& n, yvec<type> const& ts) {
		return type(std::make_shared<cons_type>(n, ts));
	}

	type type::create_set(type const& a, type const& b) {
		return type(std::make_shared<set_type>(yvec<type>{ a, b }));
	}

	type type::create_set(yvec<type> const& ts) {
		return type(std::make_shared<set_type>(ts));
	}

	type type::create_fn(type const& args, type const& res) {
		return type(std::make_shared<cons_type>("@fn", yvec<type>{ args, res }));
	}

	type type::create_tup(yvec<type> const& ts) {
		return type(std::make_shared<cons_type>("@tup", ts));
	}

	bool type::matches(type& t) {
		return match(Data, t.Data) (
			[&](auto&, auto&) -> bool { return true; },
			[&](ysptr<cons_type> t1, ysptr<cons_type> t2) -> bool {
			return bind(t1->as(), t2->as(), [&](ystr& name1, yvec<type>& types1, ystr& name2, yvec<type>& types2) {
				if (name1 != name2) {
					return false;
				}
				if (types1.size() != types2.size()) {
					return false;
				}
				if (name1 == "@fn") {
					// TODO: assert types.size() == 2
					// Just compare args
					return types1[0].matches(types2[0]);
				}
				for (ysize i = 0; i < types1.size(); i++) {
					if (!types1[i].matches(types2[i])) {
						return false;
					}
				}
				return true;
			}); },
			[&](ysptr<cons_type> t1, ysptr<set_type> t2) -> bool {
			return bind(t2->as(), [&](yvec<type>& stypes) {
				for (auto& tt : stypes) {
					if (tt.matches(*this)) {
						return true;
					}
				}
				return false;
			});},
			[&](ysptr<set_type> t1, ysptr<cons_type> t2) -> bool {
				// Rely on above
				return t.matches(*this);
			}
		);
	}

	bool type::contains(ysptr<var_type> t) {
		auto& pr = prune();
		return bind(t->as(), [&](ysize& id1, yopt<type>& instance1) {
		return match(pr.Data) (
			[&](ysptr<var_type> v) -> bool {
			return bind(v->as(), [&](ysize& id2, yopt<type>& instance2) {
				return id1 == id2;
			}); },
			[&](ysptr<cons_type> v) -> bool {
			return bind(v->as(), [&](ystr& name, yvec<type>& types) {
				for (auto& tt : types) {
					if (tt.contains(t)) {
						return true;
					}
				}
				return false;
			}); },
			[&](ysptr<set_type> v) -> bool {
			return bind(v->as(), [&](yvec<type>& types) {
				for (auto& tt : types) {
					if (tt.contains(t)) {
						return true;
					}
				}
				return false;
			}); }
		); });
	}

	ystr type::to_str() const {
		return match(Data) (
			[&](ysptr<var_type> v) -> ystr {
			return bind(v->as(), [&](ysize& id, yopt<type>& instance) {
				if (instance) {
					return instance->to_str();
				}
				return ystr{ '\'', char('A' + id % 26) };
			}); },
			[&](ysptr<cons_type> v) -> ystr {
			return bind(v->as(), [&](ystr& name, yvec<type>& types) {
				ystr res = name;
				if (types.size()) {
					res += '<' + types[0].to_str();
					for (ysize i = 1; i < types.size(); i++) {
						res += ", " + types[i].to_str();
					}
					res += '>';
				}
				return res;
			}); },
			[&](ysptr<set_type> v) -> ystr {
			return bind(v->as(), [&](yvec<type>& types) {
				ystr res = "{";
				if (types.size()) {
					res += types[0].to_str();
					for (ysize i = 1; i < types.size(); i++) {
						res += ", " + types[i].to_str();
					}
				}
				res += '}';
				return res;
			}); }
		);
	}

	type& type::prune() {
		return match(Data) (
			[&](ysptr<var_type> v) -> type& {
			return bind(v->as(), [&](ysize& id, yopt<type>& instance) -> type& {
				if (instance) {
					instance = instance->prune();
					return *instance;
				}
				return *this;
			}); },
			[&](ysptr<set_type> v) -> type& {
			return bind(v->as(), [&](yvec<type>& types) -> type& {
				if (types.size() == 1) {
					return types[0];
				}
				return *this;
			}); },
			[&](auto&) -> type& {
				return *this;
			}
		);
	}

	bool type::same(type& t) {
		auto& t1 = prune();
		auto& t2 = t.prune();

		return match(t1.Data, t2.Data) (
			[&](ysptr<var_type> a, ysptr<var_type> b) -> bool { 
			return bind(a->as(), b->as(), [&](ysize& id1, yopt<type>& instance1, ysize& id2, yopt<type>& instance2) {
				return id1 == id2;
			}); },
			[&](ysptr<cons_type> a, ysptr<cons_type> b) -> bool {
			return bind(a->as(), b->as(), [&](ystr& name1, yvec<type>& types1, ystr& name2, yvec<type>& types2) {
				if (name1 != name2) {
					return false;
				}
				if (types1.size() != types2.size()) {
					return false;
				}
				for (ysize i = 0; i < types1.size(); i++) {
					if (!types1[i].same(types2[i])) {
						return false;
					}
				}
				return true;
			}); },
			[&](ysptr<set_type> a, ysptr<set_type> b) -> bool {
			return bind(a->as(), b->as(), [&](yvec<type>& types1, yvec<type>& types2) {
				if (types1.size() != types2.size()) {
					return false;
				}
				for (ysize i = 0; i < types1.size(); i++) {
					if (!types1[i].same(types2[i])) {
						return false;
					}
				}
				return true;
			}); },
			[&](auto&, auto&) -> bool { return false; }
		);
	}

	type type::clone() {
		auto& tt = prune();
		return match(tt.Data) (
			[&](ysptr<var_type> v) {
				return create_var();
			},
			[&](ysptr<cons_type> v) {
			return bind(v->as(), [&](ystr& name, yvec<type>& types) {
				yvec<type> newtypes;
				for (auto& nt : types) {
					newtypes.push_back(nt.clone());
				}
				return create_cons(name, newtypes);
			}); },
			[&](ysptr<set_type> v) {
			return bind(v->as(), [&](yvec<type>& types) {
				yvec<type> newtypes;
				for (auto& nt : types) {
					newtypes.push_back(nt.clone());
				}
				return create_set(newtypes);
			}); }
		);
	}
}
