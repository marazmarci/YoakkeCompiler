#pragma once

#include <functional>
#include "../common.h"

namespace yk {
	namespace alg {
		template <typename ABS_T, typename BIN_T>
		static void flatten_impl(yvec<ABS_T*>& res, ABS_T* abs, ystr const& what) {
			if (auto bt = dynamic_cast<BIN_T*>(abs)) {
				if (bt->OP.identifier() == what) {
					flatten_impl<ABS_T, BIN_T>(res, bt->LHS, what);
					flatten_impl<ABS_T, BIN_T>(res, bt->RHS, what);
					return;
				}
			}
			res.push_back(abs);
		}

		template <typename ABS_T, typename BIN_T>
		auto flatten(ABS_T* abs, ystr const& what) {
			yvec<ABS_T*> res;
			flatten_impl<ABS_T, BIN_T>(res, abs, what);
			return res;
		}
	}
}