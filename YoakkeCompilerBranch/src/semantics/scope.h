#pragma once

#include "../common.h"

namespace yk {
	class symbol;

	typedef yvec<symbol*> sym_set;
	typedef ymap<ystr, sym_set> sym_dict;

	class scope {
	public:
		scope* Parent;
		yvec<scope*> Children;
		sym_dict Dict;

	public:
		virtual sym_set* ref(ystr const& id);
		void decl(symbol* sym);
	};
}