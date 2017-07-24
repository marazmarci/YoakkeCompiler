#pragma once

#include "token_input.h"
#include "../common.h"

namespace combinator {
	template <typename T>
	using return_t = yopt<ytup<T, token_input>>;

	namespace internal__ {
		template <typename Fn, typename... Args, ysize... I>
		auto call_func(Fn func, ytup<Args...>& args, std::index_sequence<I...>) {
			return func(std::get<I>(args)...);
		}

		template <typename Fn, typename... Args>
		auto delayed_dispatch(Fn func, ytup<Args...>& args) {
			return call_func(func, args, std::index_sequence_for<Args...>{});
		}
	}

	template <typename Fn, typename... Ts>
	auto apply(Fn func, ytup<Ts...>& args) {
		return internal__::delayed_dispatch(func, args);
	}

	template <typename T>
	auto success(T const& val) {
		return [=](token_input& in) -> return_t<T> {
			return std::make_tuple(val, in);
		};
	}

	template <typename T>
	auto fail() {
		return [](token_input& in) -> return_t<T> {
			return {};
		};
	}

	namespace internal__ {
		template <typename... Fns>
		struct sequence;

		template <>
		struct sequence<> {
			using result_t = ytup<>;

			static auto create() {
				return success(std::make_tuple());
			}
		};

		template <typename Fn, typename... Fns>
		struct sequence<Fn, Fns...> {
			using result_t = decltype(
				std::tuple_cat(
					std::declval<ytup<
						std::tuple_element_t<0,
						typename std::result_of_t<Fn(token_input&)>::value_type
					>>>(),
					std::declval<typename sequence<Fns...>::result_t>()
				));

			static auto create(Fn func, Fns... rest) {
				return [=](token_input& in) -> return_t<result_t> {
					if (auto res = func(in)) {
						auto& res_u = *res;
						auto& head = std::get<0>(res_u);
						auto& in2 = std::get<1>(res_u);
						if (auto res2 = sequence<Fns...>::create(rest...)(in2)) {
							auto& res2_u = *res2;
							auto& tail_tup = std::get<0>(res2_u);
							auto& in3 = std::get<1>(res2_u);
							return std::make_tuple(std::tuple_cat(
									std::make_tuple(head), tail_tup
							), in3);
						}
						else {
							return {};
						}
					}
					else {
						return {};
					}
				};
			}
		};
	}

	template <typename... Fns>
	auto sequence(Fns... funcs) {
		return internal__::sequence<Fns...>::create(funcs...);
	}

	namespace internal__ {
		template <typename... Fns>
		struct either;

		template <typename Fn>
		struct either<Fn> {
			using helper_t =
				std::tuple_element_t<0,
				typename std::result_of_t<Fn(token_input&)>::value_type>;

			static auto create(Fn func) {
				return [=](token_input& in) -> return_t<helper_t> {
					if (auto res = func(in)) {
						auto& res_u = *res;
						auto& left = std::get<0>(res_u);
						auto& in2 = std::get<1>(res_u);
						return std::make_tuple(left, in2);
					}
					else {
						return {};
					}
				};
			}
		};

		template <typename Fn, typename... Fns>
		struct either<Fn, Fns...> {
			using helper_t =
				std::tuple_element_t<0,
				typename std::result_of_t<Fn(token_input&)>::value_type>;

			static auto create(Fn func, Fns... rest) {
				return [=](token_input& in) -> return_t<helper_t> {
					if (auto res = func(in)) {
						auto& res_u = *res;
						auto& left = std::get<0>(res_u);
						auto& in2 = std::get<1>(res_u);
						return std::make_tuple(left, in2);
					}
					else {
						return either<Fns...>::create(rest...)(in);
					}
				};
			}
		};
	}

	template <typename... Fns>
	auto either(Fns... funcs) {
		return internal__::either<Fns...>::create(funcs...);
	}

	template <token_t TokT>
	auto terminal() {
		return [](token_input& in) -> return_t<token> {
			token& tok = in.head();
			if (tok.Type == TokT) {
				return std::make_tuple(tok, in.tail());
			}
			else {
				return {};
			}
		};
	}
}
