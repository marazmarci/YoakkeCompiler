#pragma once

#include "../common.h"
#include "../lexing/position.h"
#include "type.h"

namespace yk {
	struct type;
	class var_sym;

	class scope {
	public:
		ysptr<scope>							Parent;
		yhash_map<ystr, yvec<ysptr<var_sym>>>	Entries;
		bool									ReturnDest;
		yopt<type>								ReturnType;
		interval								ReturnPos;

	public:
		scope();

	public:
		ysptr<var_sym> ref(ystr const& id);
		ypair<ysptr<var_sym>, bool> ref(ystr const& id, type& hint);
		void decl(ysptr<var_sym> var);
		scope* enclosing_return();
	};
}
