#pragma once

#include "../common.h"
#include "../lexing/position.h"
#include "type.h"

namespace yk {
	struct type;
	struct symbol;

	class scope {
	public:
		ysptr<scope>			Parent;
		yhash_map<ystr, symbol>	Entries;
		bool					ReturnDest;
		yopt<type>				ReturnType;
		interval				ReturnPos;

	public:
		scope();

	public:
		yopt<symbol> ref(ystr const& id);
		yopt<yhash_map<ystr, symbol>::iterator> ref_it(ystr const& id);
		void decl(symbol const& var);
		scope* enclosing_return();
	};

	class ty_scope {
	public:
		ysptr<ty_scope>				Parent;
		yhash_map<ystr, type>		Entries;

	public:
		ty_scope();

	public:
		yopt<type> ref(ystr const& id);
		void decl(ystr const& id, type& ty);
	};
}
