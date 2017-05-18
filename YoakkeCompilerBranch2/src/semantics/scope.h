#pragma once

#include "../common.h"
#include "../lexing/position.h"

namespace yk {
	class type;
	class var_sym;

	class scope {
	public:
		ysptr<scope>							Parent;
		yhash_map<ystr, yvec<ysptr<var_sym>>>	Entries;
		bool									ReturnDest;
		ysptr<type>								ReturnType;
		interval								ReturnPos;

	public:
		scope();

	public:
		ysptr<var_sym> ref(ystr const& id);
		ypair<ysptr<var_sym>, bool> ref(ystr const& id, ysptr<type> hint);
		void decl(ysptr<var_sym> var);
		scope* enclosing_return();
	};
}
