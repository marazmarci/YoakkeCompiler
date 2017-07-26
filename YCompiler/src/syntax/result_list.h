#pragma once

#include "../common.h"

namespace combinator {
	template <typename... Ts>
	struct result_list : public ytup<Ts...> {
		using tuple_t = ytup<Ts...>;

		using ytup<Ts...>::ytup;

		tuple_t& as_tuple() {
			return *this;
		}

		tuple_t const& as_tuple() const {
			return *this;
		}
	};

	namespace internal__ {
		template <typename... Ts>
		auto result_list_constructor(Ts&&... elems) {
			return result_list<Ts...>(std::forward<Ts>(elems)...);
		}

		template <typename Tup, ysize... Indicies>
		auto result_list_t_constructor(Tup&& tup, std::index_sequence<Indicies...>) {
			return result_list_constructor(std::get<Indicies>(std::forward<Tup>(tup))...);
		}
	}

	template <typename... Ts>
	auto make_result_list(Ts&&... elems) {
		return internal__::result_list_constructor(std::forward<Ts>(elems)...);
	}

	template <typename Tup>
	auto make_result_list_from_tuple(Tup&& tup) {
		using Seq = std::make_index_sequence<std::tuple_size<Tup>::value>;
		return internal__::result_list_t_constructor(
			std::forward<Tup>(tup), 
			Seq{}
		);
	}
}
