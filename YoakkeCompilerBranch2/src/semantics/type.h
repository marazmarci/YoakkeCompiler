#pragma once

#include "../common.h"
#include "../utility/type_tag.h"

#define make_type(x, ...) make_tag(x, type, __VA_ARGS__)

namespace yk {
	struct type;

	make_type(var, ysize, yopt<type>);
	make_type(cons, ystr, yvec<type>);

	using tty_ty = yvar <
		  ysptr<var_type>
		, ysptr<cons_type>
	>;

	struct type {
	private:
		static ysize s_IDcount;

	public:
		tty_ty Data;

	public:
		template <typename... Ts>
		type(Ts&&... xs)
			: Data(std::forward<Ts>(xs)...) {
		}

		type(tty_ty const& tt)
			: Data(tt) {
		}

	public:
		bool matches(type const& t) const;
		bool contains(ysptr<var_type> t);
		ystr to_str() const;
		type& prune();
		bool same(type& t);

	public:
		static type create_var();
		static type create_cons(ystr const& n, yvec<type> const& ts = yvec<type>{});
		static type create_fn(type const& args, type const& res);
		static type create_tup(yvec<type> const& ts);
	};
}
