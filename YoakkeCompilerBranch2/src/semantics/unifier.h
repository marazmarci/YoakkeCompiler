#pragma once

#include "../common.h"

namespace yk {
	struct type;

	struct unifier {
	public:
		struct err {
		public:
			ystr Msg;

		public:
			err(ystr const& m);
		};

	public:
		unifier() = delete;

	public:
		static void unify(type& t1, type& t2);
	};
}
