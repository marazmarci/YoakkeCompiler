#pragma once

#include "../common.h"

#define make_node_(n, ...) using n = node_tag<ytup<__VA_ARGS__>, struct CAT(n, _tag)>
#define make_node(x, y, ...) make_node_(CAT(x, CAT(_, y)), __VA_ARGS__)

namespace yk {
	template <typename T, typename Tag>
	struct node_tag : public T {
		node_tag()
			: T() {
		}

		template <typename... Ts>
		node_tag(Ts&&... args)
			: T(std::forward<Ts>(args)...) {
		}

		template <ysize I>
		constexpr auto& get() {
			return std::get<I>(*this);
		}
	};
}
