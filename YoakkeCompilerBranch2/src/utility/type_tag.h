#pragma once

#include "../common.h"

#define make_tag_(n, ...) using n = type_tag<ytup<__VA_ARGS__>, struct CAT(n, _tag)>
#define make_tag(x, y, ...) make_tag_(CAT(x, CAT(_, y)), __VA_ARGS__)

namespace yk {
	template <typename T, typename Tag>
	struct type_tag : public T {
		template <typename... Ts>
		type_tag(Ts&&... args)
			: T(std::forward<Ts>(args)...) {
		}

		T& as() {
			return static_cast<T&>(*this);
		}
	};
}
