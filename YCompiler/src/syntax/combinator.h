#pragma once

#include <functional>
#include <type_traits>
#include "result_list.h"
#include "token_input.h"
#include "../common.h"

namespace combinator {
	struct fail_info;
	
	template <typename T>
	using return_t = yresult<ytup<T, token_input>, fail_info>;

	template <typename T>
	using parser_t = std::function<return_t<T>(token_input&)>;

	struct fail_info { };

	template <typename T, typename U>
	auto operator>=(parser_t<T> fn1, parser_t<U> fn2) {
		using fn1_t = decltype(fn1);
		using left_ok_t = std::result_of_t<fn1_t(token_input&)>::ok_type;
		using left_t = std::tuple_element_t<0, left_ok_t>;

		using fn2_t = decltype(fn2);
		using right_ok_t = std::result_of_t<fn2_t(token_input&)>::ok_type;
		using right_t = std::tuple_element_t<0, right_ok_t>;

		using success_t = decltype(
			cat_results(std::declval<left_t>(), std::declval<right_t>())
		);

		return parser_t<success_t>([=](token_input& in) -> return_t<success_t> {
			auto result = fn1(in);
			if (result.is_ok()) {
				auto& result_ok = result.get_ok();
				auto& left = std::get<0>(result_ok);
				auto& in2 = std::get<1>(result_ok);

				auto result2 = fn2(in2);
				if (result2.is_ok()) {
					auto& result2_ok = result2.get_ok();
					auto& right = std::get<0>(result2_ok);
					auto& in3 = std::get<1>(result2_ok);

					return std::make_tuple(
						cat_results(left, right),
						in3
					);
				}
				else {
					return result2.get_err();
				}
			}
			else {
				return result.get_err();
			}
		});
	}

	template <typename T, typename U>
	auto operator>(parser_t<T> fn1, parser_t<U> fn2) {
		using fn2_t = decltype(fn2);
		using right_ok_t = std::result_of_t<fn2_t(token_input&)>::ok_type;
		using right_t = std::tuple_element_t<0, right_ok_t>;

		return parser_t<right_t>([=](token_input& in) -> return_t<right_t> {
			auto result = fn1(in);
			if (result.is_ok()) {
				auto& result_ok = result.get_ok();
				auto& in2 = std::get<1>(result_ok);

				auto result2 = fn2(in2);
				if (result2.is_ok()) {
					auto& result2_ok = result2.get_ok();
					auto& right = std::get<0>(result2_ok);
					auto& in3 = std::get<1>(result2_ok);

					return std::make_tuple(
						right,
						in3
					);
				}
				else {
					return result2.get_err();
				}
			}
			else {
				return result.get_err();
			}
		});
	}

	template <typename T, typename U>
	auto operator<(parser_t<T> fn1, parser_t<U> fn2) {
		using fn1_t = decltype(fn1);
		using left_ok_t = std::result_of_t<fn1_t(token_input&)>::ok_type;
		using left_t = std::tuple_element_t<0, left_ok_t>;

		return parser_t<left_t>([=](token_input& in) -> return_t<left_t> {
			auto result = fn1(in);
			if (result.is_ok()) {
				auto& result_ok = result.get_ok();
				auto& left = std::get<0>(result_ok);
				auto& in2 = std::get<1>(result_ok);

				auto result2 = fn2(in2);
				if (result2.is_ok()) {
					auto& result2_ok = result2.get_ok();
					auto& in3 = std::get<1>(result2_ok);

					return std::make_tuple(
						left,
						in3
					);
				}
				else {
					return result2.get_err();
				}
			}
			else {
				return result.get_err();
			}
		});
	}

	template <token_t TokenT>
	parser_t<token> terminal() {
		return [](token_input& in) -> return_t<token> {
			token tok = in.head();
			if (tok.Type == TokenT) {
				token_input in2 = in.tail();
				return std::make_tuple(tok, in2);
			}
			else {
				// TODO:
				return return_t<token>(fail_info());
			}
		};
	}
}