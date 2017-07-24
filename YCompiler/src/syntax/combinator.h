#pragma once

#include "token_input.h"
#include "../common.h"

namespace combinator {
	template <typename T>
	using return_t = yopt<ytup<T, token_input>>;

	template <typename T>
	auto success(T const& val) {
		return [=](token_input& in) -> return_t<T> {
			return std::make_tuple(val, in);
		}
	}

	template <typename T>
	auto fail() {
		return []() -> return_t<T> {
			return {};
		}
	}

	template <token_t TT>
	auto terminal() {
		return [](token_input& in) -> return_t<token> {
			token tok = in.head();
			if (tok.Type == TT) {
				return std::make_tuple(tok, in.tail());
			}
			else {
				return {};
			}
		};
	}

	namespace internal__ {
		template <token_t... Toks>
		struct terminal_seq_helper;

		template <>
		struct terminal_seq_helper<> {
			using tuple_t = ytup<>;

			static auto create() {
				return [](token_input& in) -> return_t<tuple_t> {
					return std::make_tuple(std::make_tuple(), in);
				};
			}
		};

		template <token_t Head, token_t... Tail>
		struct terminal_seq_helper<Head, Tail...> {
			using tuple_t = decltype(
				std::tuple_cat(
					std::declval<ytup<token>>(), 
					std::declval<terminal_seq_helper<Tail...>::tuple_t>()));

			static auto create() {
				return[](token_input& in) -> return_t<tuple_t> {
					if (auto result = terminal<Head>()(in)) {
						auto& result_unw = *result;
						token tok_head = std::get<0>(result_unw);
						auto& in2 = std::get<1>(result_unw);
						if (auto result2 = terminal_seq_helper<Tail...>::create()(in2)) {
							auto& result2_unw = *result2;
							auto& tup_result = std::get<0>(result2_unw);
							auto& in3 = std::get<1>(result2_unw);
							return std::make_tuple(std::tuple_cat(
								std::make_tuple(tok_head),
								tup_result), in3);
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

	template <token_t... TTs>
	auto terminal_seq() {
		return internal__::terminal_seq_helper<TTs...>::create();
	}
}
