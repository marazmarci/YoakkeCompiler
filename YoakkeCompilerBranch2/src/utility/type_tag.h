#pragma once

#include "../common.h"

#define make_tag_(n, ...) using n = type_tag<ytup<__VA_ARGS__>, struct CAT(n, _tag)>
#define make_tag(x, y, ...) make_tag_(CAT(x, CAT(_, y)), __VA_ARGS__)

namespace yk {
	template <typename T, typename Tag>
	struct type_tag : public T {
		type_tag()
			: T() {
		}

		template <typename... Ts>
		type_tag(Ts&&... args)
			: T(std::forward<Ts>(args)...) {
		}

		template <ysize I>
		constexpr auto& get() {
			return std::get<I>(*this);
		}
	};
}
