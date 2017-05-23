#pragma once

#include "type.h"
#include "../common.h"
#include "../utility/type_tag.h"

#define make_sym(x, ...) make_tag(x, symbol, __VA_ARGS__)

namespace yk {
	struct symbol;

	make_sym(variable);
	make_sym(constant);

	struct symbol {
	public:
		ystr Name;
		type Type;
		yvar<
			  ysptr<variable_symbol>
			, ysptr<constant_symbol>
		> Data;

	public:
		template <typename T>
		symbol(ystr const& n, type const& t, ysptr<T> s)
			: Name(n), Type(t), Data(s) {
		}

		symbol(symbol const& ss)
			: Name(ss.Name), Type(ss.Type), Data(ss.Data) {
		}

	public:
		static symbol create_constant(ystr const& n, type const& t);
		static symbol create_variable(ystr const& n, type const& t);
	};
}
