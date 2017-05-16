#pragma once

#include "../common.h"

namespace yk {
	struct type;
	struct var_sym;

	class scope {
	public:
		ysptr<scope>							Parent;
		yhash_map<ystr, yvec<ysptr<var_sym>>>	Entries;

	public:
		scope();

	public:
		ysptr<var_sym> ref(ystr const& id);
		ysptr<var_sym> ref(ystr const& id, ysptr<type> hint);
		void decl(ysptr<var_sym> var);
	};
}
