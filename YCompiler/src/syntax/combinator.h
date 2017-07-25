#pragma once

#include <functional>
#include <type_traits>
#include "token_input.h"
#include "../common.h"

namespace combinator {
	struct fail_info;

	template <typename... Ts>
	using success_t = ytup<ytup<Ts...>, token_input>;

	template <typename... Ts>
	using result_t = yresult<success_t<Ts...>, fail_info>;

	template <typename... Ts>
	using parser_t = std::function<result_t<Ts...>(token_input&)>;

	template <typename T>
	using parser_tt = std::function<T(token_input&)>;

	struct fail_info {

	};
	
	template <typename... Ts>
	parser_t<Ts...> success(Ts const&... vals) {
		return [=](token_input& in) -> result_t<Ts...> {
			return std::make_tuple(std::tuple_cat(vals...), in);
		};
	}

	template <typename... Ts>
	parser_t<Ts...> fail() {
		return [](token_input& in) -> result_t<Ts...> {
			return fail_info();
		};
	}

	template <typename Fn, typename... Ts>
	auto wrap(Fn wrapper, parser_t<Ts...> parser) {
		using wrap_t = std::result_of_t<Fn(ytup<Ts...>&)>;
		return parser_t<wrap_t>(
			[=](token_input& in) -> result_t<wrap_t> {
				auto result = parser(in);
				if (result.is_ok()) {
					auto& result_ok = result.get_ok();
					auto& value = std::get<0>(result_ok);
					auto& in2 = std::get<1>(result_ok);
					return std::make_tuple(
						wrapper(value),
						in2
					);
				}
				else {
					// TODO
					return result_t<wrap_t>(result.get_err());
				}
			}
		);
	}

	template <typename... Ts1, typename... Ts2>
	parser_t<Ts1..., Ts2...> operator>=(parser_t<Ts1...> fn1, parser_t<Ts2...> fn2) {
		return [=](token_input& in) -> result_t<Ts1..., Ts2...> {
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
					return result_t<Ts1..., Ts2...>(std::make_tuple(
						std::tuple_cat(left, right),
						in3
					));
				}
				else {
					// TODO
					return result_t<Ts1..., Ts2...>(result2.get_err());
				}
			}
			else {
				// TODO
				return result_t<Ts1..., Ts2...>(result.get_err());
			}
		};
	}

	template <typename... Ts1, typename... Ts2>
	parser_t<Ts2...> operator>(parser_t<Ts1...> fn1, parser_t<Ts2...> fn2) {
		return [=](token_input& in) -> result_t<Ts2...> {
			auto result = fn1(in);
			if (result.is_ok()) {
				auto& result_ok = result.get_ok();
				auto& in2 = std::get<1>(result_ok);

				auto result2 = fn2(in2);
				if (result2.is_ok()) {
					auto& result2_ok = result2.get_ok();
					auto& right = std::get<0>(result2_ok);
					auto& in3 = std::get<1>(result2_ok);
					return result_t<Ts2...>(std::make_tuple(
						right,
						in3
					));
				}
				else {
					// TODO
					return result_t<Ts2...>(result2.get_err());
				}
			}
			else {
				return result_t<Ts2...>(result.get_err());
			}
		};
	}

	template <typename... Ts1, typename... Ts2>
	parser_t<Ts2...> operator<(parser_t<Ts1...> fn1, parser_t<Ts2...> fn2) {
		return [=](token_input& in) -> result_t<Ts1...> {
			auto result = fn1(in);
			if (result.is_ok()) {
				auto& result_ok = result.get_ok();
				auto& left = std::get<0>(result_ok);
				auto& in2 = std::get<1>(result_ok);

				auto result2 = fn2(in2);
				if (result2.is_ok()) {
					auto& result2_ok = result2.get_ok();
					auto& in3 = std::get<1>(result2_ok);
					return result_t<Ts1...>(std::make_tuple(
						left,
						in3
					));
				}
				else {
					// TODO
					return result_t<Ts1...>(result2.get_err());
				}
			}
			else {
				// TODO
				return result_t<Ts1...>(result.get_err());
			}
		};
	}

	template <typename... Ts>
	parser_t<Ts...> operator|(parser_t<Ts...> fn1, parser_t<Ts...> fn2) {
		return [=](token_input& in) -> result_t<Ts...> {
			// TODO: check for ambiguity by applying both
			auto result = fn1(in);
			if (result.is_ok()) {
				return result;
			}
			auto result2 = fn2(in);
			if (result2.is_ok()) {
				return result2;
			}
			// TODO
			return result_t<Ts...>(fail_info());
		};
	}

	template <typename... Ts>
	parser_tt<result_t<yvec<ytup<Ts...>>>> operator*(parser_t<Ts...> fn) {
		using elem_t = ytup<Ts...>;
		using ret_t = result_t<yvec<elem_t>>;
		return [=](token_input& in) -> ret_t {
			yvec<elem_t> result_list;
			token_input* in2 = new token_input(in);
			while (true) {
				auto result = fn(*in2);
				if (result.is_ok()) {
					auto& result_ok = result.get_ok();
					auto& elem = std::get<0>(result_ok);
					auto& in3 = std::get<1>(result_ok);
					result_list.push_back(elem);
					delete in2;
					in2 = new token_input(in3);
				}
				else {
					break;
				}
			}
			return std::make_tuple(
				result_list,
				*in2);
		};
	}

	template <token_t TokenT>
	parser_t<token> terminal() {
		return [](token_input& in) -> result_t<token> {
			token tok = in.head();
			if (tok.Type == TokenT) {
				token_input in2 = in.tail();
				return std::make_tuple(std::make_tuple(tok), in2);
			}
			else {
				// TODO:
				return result_t<token>(fail_info());
			}
		};
	}
}
