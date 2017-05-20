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
				auto& name1		= t1->get<0>();
				auto& types1	= t1->get<1>();
				auto& name2		= t2->get<0>();
				auto& types2	= t2->get<1>();

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
			}
		);
	}

	bool type::contains(ysptr<var_type> t) {
		auto& pr = prune();
		return match(pr.Data) (
			[&](ysptr<var_type> v) -> bool {
				return t->get<0>() == v->get<0>();
			},
			[&](ysptr<cons_type> v) -> bool {
				auto& types = v->get<1>();
				for (auto& tt : types) {
					if (tt.contains(t)) {
						return true;
					}
				}
				return false;
			}
		);
	}

	ystr type::to_str() const {
		return match(Data) (
			[&](ysptr<var_type> v) -> ystr {
				auto& id = v->get<0>();
				auto& inst = v->get<1>();
				if (inst) {
					return inst->to_str();
				}
				return ystr{ '\'', char('A' + id % 26) };
			},
			[&](ysptr<cons_type> v) -> ystr {
				ystr res = v->get<0>();
				auto& types = v->get<1>();
				if (types.size()) {
					res += '<' + types[0].to_str();
					for (ysize i = 1; i < types.size(); i++) {
						res += ", " + types[i].to_str();
					}
					res += '>';
				}
				return res;
			}
		);
	}

	type& type::prune() {
		return match(Data) (
			[&](ysptr<var_type> v) -> type& {
				yopt<type>& inst = v->get<1>();
				if (inst) {
					inst = inst->prune();
					return *inst;
				}
				return *this;
			},
			[&](ysptr<cons_type> v) -> type& {
				return *this;
			}
		);
	}

	bool type::same(type& t) {
		auto& t1 = prune();
		auto& t2 = t.prune();

		return match(t1.Data, t2.Data) (
			[&](ysptr<var_type> a, ysptr<var_type> b) -> bool { 
				return a->get<0>() == b->get<0>(); 
			},
			[&](ysptr<cons_type> a, ysptr<cons_type> b) -> bool {
				auto& name1			= a->get<0>();
				yvec<type>& types1	= a->get<1>();
				auto& name2			= b->get<0>();
				yvec<type>& types2	= b->get<1>();

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
			},
			[&](auto&, auto&) -> bool { return false; }
		);
	}
}
