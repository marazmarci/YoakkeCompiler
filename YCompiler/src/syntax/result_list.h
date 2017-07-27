#pragma once

#include <type_traits>
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

	template <typename T, typename U>
	auto cat_result_lists(T&& l1, U&& l2) {
		return make_result_list_from_tuple(
			std::tuple_cat(
				std::forward<typename std::decay_t<T>::tuple_t>(l1.as_tuple()),
				std::forward<typename std::decay_t<U>::tuple_t>(l2.as_tuple())
			)
		);
	}

	template <typename T, typename U>
	auto cat_result_list_with_element(T&& l1, U&& l2) {
		return make_result_list_from_tuple(
			std::tuple_cat(
				std::forward<typename std::decay_t<T>::tuple_t>(l1.as_tuple()),
				std::make_tuple(std::forward<U>(l2))
			)
		);
	}

	template <typename T, typename U>
	auto cat_element_with_result_list(T&& l1, U&& l2) {
		return make_result_list_from_tuple(
			std::tuple_cat(
				std::make_tuple(std::forward<T>(l1)),
				std::forward<typename std::decay_t<U>::tuple_t>(l2.as_tuple())
			)
		);
	}

	namespace internal__ {
		template <bool A, bool B, typename T, typename U>
		struct cat_results_impl;

		template <typename T, typename U>
		struct cat_results_impl<true, true, T, U> {
			static auto cat(T&& x, U&& y) {
				return cat_result_lists(std::forward<T>(x), std::forward<U>(y));
			}
		};

		template <typename T, typename U>
		struct cat_results_impl<true, false, T, U> {
			static auto cat(T&& x, U&& y) {
				return cat_result_list_with_element(std::forward<T>(x), std::forward<U>(y));
			}
		};

		template <typename T, typename U>
		struct cat_results_impl<false, true, T, U> {
			static auto cat(T&& x, U&& y) {
				return cat_element_with_result_list(std::forward<T>(x), std::forward<U>(y));
			}
		};

		template <typename T, typename U>
		struct cat_results_impl<false, false, T, U> {
			static auto cat(T&& x, U&& y) {
				return make_result_list(std::forward<T>(x), std::forward<U>(y));
			}
		};
	}

	template <typename Test, template <typename...> class Ref>
	struct is_specialization : std::false_type {};

	template <template <typename...> class Ref, typename... Args>
	struct is_specialization<Ref<Args...>, Ref> : std::true_type {};

	template <typename T, typename U>
	auto cat_results(T&& a, U&& b) {
		using TT = std::decay_t<T>;
		using UU = std::decay_t<U>;
		constexpr bool A = is_specialization<TT, result_list>::value;
		constexpr bool B = is_specialization<UU, result_list>::value;

		return internal__::cat_results_impl<A, B, TT, UU>::cat(
			std::forward<TT>(a), std::forward<UU>(b)
		);
	}

	namespace internal__ {
		template <bool B, typename Fn, typename T>
		struct apply_result_impl;

		template <typename Fn, typename T>
		struct apply_result_impl<true, Fn, T> {
			static auto apply(Fn&& func, T&& res) {
				// TODO: forward?
				return	std::apply(
					std::forward<Fn>(func), 
					res.as_tuple()
				);
			}
		};

		template <typename Fn, typename T>
		struct apply_result_impl<false, Fn, T> {
			static auto apply(Fn&& func, T&& res) {
				// TODO: forward
				return func(res);
			}
		};
	}

	template <typename Fn, typename T>
	auto apply_result(Fn&& func, T&& res) {
		//using FFn = std::decay_t<Fn>;
		using TT = std::decay_t<T>;
		constexpr bool A = is_specialization<TT, result_list>::value;
		return internal__::apply_result_impl<A, Fn, TT>::apply(
			std::forward<Fn>(func), 
			std::forward<TT>(res)
		);
	}
}
