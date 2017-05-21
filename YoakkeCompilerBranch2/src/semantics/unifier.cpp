#include "unifier.h"
#include "type.h"
#include "../utility/match.h"

namespace yk {
	unifier::err::err(ystr const& m)
		: Msg(m) {
	}

	void unifier::unify(type& t1, type& t2) {
		t1 = t1.prune();
		t2 = t2.prune();

		match(t1.Data, t2.Data) (
			[&](ysptr<cons_type> tt1, ysptr<cons_type> tt2) {
			bind(tt1->as(), tt2->as(), [&](ystr& name1, yvec<type>& types1, ystr& name2, yvec<type>& types2) {
				if (name1 != name2) {
					throw err(t1.to_str() + " is not " + t2.to_str());
				}
				if (types1.size() != types2.size()) {
					throw err(t1.to_str() + " has different number of components as " + t2.to_str());
				}
				for (ysize i = 0; i < types1.size(); i++) {
					unify(types1[i], types2[i]);
				}
			}); },
			[&](ysptr<var_type> tt1, ysptr<var_type> tt2) {
			bind(tt2->as(), [&](ysize& id, yopt<type>& instance) {
				if (t1.same(t2)) {
					return;
				}
				instance = t1;
			}); },
			[&](ysptr<cons_type> tt1, ysptr<var_type> tt2) {
			bind(tt2->as(), [&](ysize& id, yopt<type>& instance) {
				if (t1.contains(tt2)) {
					throw err("Recursive type: " +  t1.to_str() + " contains " + t2.to_str());
				}
				instance = t1;
			}); },
			[&](ysptr<var_type> tt1, ysptr<cons_type> tt2) {
			bind(tt1->as(), [&](ysize& id, yopt<type>& instance) {
				// Back to the other handler
				unify(t2, t1);
			}); }
		);
	}
}
