#pragma once

#include <iostream>
#include <functional>
#include <memory>
#include <type_traits>
#include "result_list.h"
#include "token_input.h"
#include "../common.h"

namespace combinator {
	template <typename ORIG, typename TO, typename... Ts>
	ORIG* construct(Ts const&... params) {
		return new TO(params...);
	}

	struct fail_info;
	
	template <typename T>
	using result_t = yresult<ytup<T, token_input>, fail_info>;

	template <typename T>
	using parser_t = std::function<result_t<T>(token_input&)>;

	struct parser_exp_tok_err {
	public:
		file_hnd const& File;
		ystr			Expected;
		token			Got;

	public:
		parser_exp_tok_err(file_hnd const& file, ystr const& exp, token const& got)
			: File(file), Expected(exp), Got(got) {
		}
	};

	using parser_err = yvar<parser_exp_tok_err>;

	struct error_t {
		yvar<lexer_err, parser_err/*, yvec<error_t>*/> Data;
	};

	struct fail_info {
		ysize	Index;
		error_t	Error;
		bool	Fatal;

		fail_info(ysize idx, lexer_err const& err)
			: Index(idx), Error{ err }, Fatal(false) {
		}

		fail_info(ysize idx, parser_err const& err)
			: Index(idx), Error{ err }, Fatal(false) {
		}

		//fail_info(ysize idx, yvec<error_t> const& err)
		//	: Index(idx), Error{ err }, Fatal(false) {
		//}

		auto const& err() const {
			return Error.Data;
		}

		auto& err() {
			return Error.Data;
		}
	};

	template <typename T, typename U>
	auto operator>=(parser_t<T> fn1, parser_t<U> fn2);
	template <typename T, typename U>
	auto operator>(parser_t<T> fn1, parser_t<U> fn2);
	template <typename T, typename U>
	auto operator<(parser_t<T> fn1, parser_t<U> fn2);
	template <typename T>
	auto operator|(parser_t<T> fn1, parser_t<T> fn2);
	template <typename T>
	auto operator*(parser_t<T> fn);
	template <typename T>
	auto operator+(parser_t<T> fn);
	template <typename T>
	auto operator&(parser_t<T> fn);
	template <typename T>
	auto operator!(parser_t<T> fn);
	template <typename T>
	auto operator-(parser_t<T> fn);
	template <typename T, typename Fn>
	auto operator^(parser_t<T> fn, Fn wrapper);
	template <token_t TokenT>
	parser_t<token> terminal(ystr const& expect);
	template <typename T>
	auto operator/(parser_t<T> fn, const char* rename);
	template <typename T>
	auto operator%(parser_t<T> fn, const char* rename);

	inline auto success(token_input& in) {
		using succ_t = result_list<>;
		
		return result_t<succ_t>(std::make_tuple(make_result_list(), in));
	}

	template <typename T, typename U>
	auto operator>=(parser_t<T> fn1, parser_t<U> fn2) {
		using fn1_t = decltype(fn1);
		using left_ok_t = typename std::result_of_t<fn1_t(token_input&)>::ok_type;
		using left_t = std::tuple_element_t<0, left_ok_t>;

		using fn2_t = decltype(fn2);
		using right_ok_t = typename std::result_of_t<fn2_t(token_input&)>::ok_type;
		using right_t = std::tuple_element_t<0, right_ok_t>;

		using succ_t = decltype(
			cat_results(std::declval<left_t>(), std::declval<right_t>())
		);

		return parser_t<succ_t>([=](token_input& in) -> result_t<succ_t> {
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
		using right_ok_t = typename std::result_of_t<fn2_t(token_input&)>::ok_type;
		using right_t = std::tuple_element_t<0, right_ok_t>;

		return parser_t<right_t>([=](token_input& in) -> result_t<right_t> {
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
		using left_ok_t = typename std::result_of_t<fn1_t(token_input&)>::ok_type;
		using left_t = std::tuple_element_t<0, left_ok_t>;

		return parser_t<left_t>([=](token_input& in) -> result_t<left_t> {
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

	template <typename T>
	auto operator|(parser_t<T> fn1, parser_t<T> fn2) {
		return parser_t<T>([=](token_input& in) -> result_t<T> {
			auto result = fn1(in);
			if (result.is_ok()) {
				return result;
			}
			else {
				auto& err1 = result.get_err();
				if (err1.Fatal) {
					return err1;
				}
				auto result2 = fn2(in);
				if (result2.is_ok()) {
					return result2;
				}
				else {
					auto& err2 = result2.get_err();
					if (err2.Fatal) {
						return err2;
					}
					if (err1.Index > err2.Index) {
						return err1;
					}
					else /* if (err2.Index > err1.Index) */ {
						return err2;
					}
					//else {
					//	return fail_info(err1.Index, { err1.Error, err2.Error });
					//}
				}
			}
		});
	}

	template <typename T>
	auto operator*(parser_t<T> fn) {
		return parser_t<yvec<T>>([=](token_input& in) -> result_t<yvec<T>> {
			yvec<T> elements;
			token_input in2 = in;
			while (true) {
				auto result = fn(in2);
				if (result.is_ok()) {
					auto& result_ok = result.get_ok();
					auto& elem = std::get<0>(result_ok);
					in2 = std::get<1>(result_ok);
					elements.push_back(elem);
				}
				else {
					auto& err = result.get_err();
					if (err.Fatal) {
						return err;
					}
					break;
				}
			}
			return std::make_tuple(elements, in2);
		});
	}

	template <typename T>
	auto operator+(parser_t<T> fn) {
		return (fn >= (*fn)) ^ [](auto& first, auto& rest) {
			rest.insert(rest.begin(), first);
			return rest;
		};
	}

	template <typename T>
	auto operator&(parser_t<T> fn) {
		return parser_t<yopt<T>>([=](token_input& in) -> result_t<yopt<T>> {
			auto result = fn(in);
			if (result.is_ok()) {
				auto& result_ok = result.get_ok();
				auto& elem = std::get<0>(result_ok);
				auto& in2 = std::get<1>(result_ok);
				return std::make_tuple(yopt<T>(elem), in2);
			}
			else {
				auto& err = result.get_err();
				if (err.Fatal) {
					return err;
				}
				return std::make_tuple(yopt<T>{}, in);
			}
		});
	}

	template <typename T>
	auto operator!(parser_t<T> fn) {
		return parser_t<T>([=](token_input& in) -> result_t<T> {
			auto result = fn(in);
			if (result.is_ok()) {
				auto& result_ok = result.get_ok();
				auto& elem = std::get<0>(result_ok);
				auto& in2 = std::get<1>(result_ok);
				return std::make_tuple(elem, in2);
			}
			else {
				auto& err = result.get_err();
				err.Fatal = true;
				return err;
			}
		});
	}

	template <typename T>
	auto operator-(parser_t<T> fn) {
		return parser_t<T>([=](token_input& in) -> result_t<T> {
			auto result = fn(in);
			if (result.is_ok()) {
				auto& result_ok = result.get_ok();
				auto& elem = std::get<0>(result_ok);
				auto& in2 = std::get<1>(result_ok);
				return std::make_tuple(elem, in2);
			}
			else {
				auto& err = result.get_err();
				err.Fatal = false;
				return err;
			}
		});
	}

	template <typename T, typename Fn>
	auto operator^(parser_t<T> fn, Fn wrapper) {
		using fn_t = decltype(fn);
		using left_ok_t = typename std::result_of_t<fn_t(token_input&)>::ok_type;
		using left_t = std::tuple_element_t<0, left_ok_t>;
		using wrapper_t = decltype(wrapper);
		using ret_t = decltype(
			apply_result(std::declval<wrapper_t>(), std::declval<left_t>())
		);
		return parser_t<ret_t>([=](token_input& in) -> result_t<ret_t> {
			auto result = fn(in);
			if (result.is_ok()) {
				auto& result_ok = result.get_ok();
				auto& value = std::get<0>(result_ok);
				auto& in2 = std::get<1>(result_ok);
				auto value_transformed = apply_result(wrapper, value);
				return std::make_tuple(value_transformed, in2);
			}
			else {
				return result.get_err();
			}
		});
	}

	template <typename T>
	auto operator/(parser_t<T> fn, const char* rename) {
		return parser_t<T>([=](token_input& in) -> result_t<T> {
			auto res = fn(in);
			if (res.is_ok()) {
				return res.get_ok();
			}
			else {
				auto& err = res.get_err();
				if (err.Fatal) {
					return err;
				}
				else {
					auto tok_res = in.head();
					if (tok_res.is_ok()) {
						return fail_info(in.get_index(),
							parser_exp_tok_err(
							in.get_lexer().file(), rename, tok_res.get_ok()
						));
					}
					else {
						return fail_info(in.get_index(),
							tok_res.get_err());
					}
				}
			}
		});
	}

	template <typename T>
	auto operator%(parser_t<T> fn, const char* rename) {
#ifdef DEBUG_GRAMMAR
		return parser_t<T>([=](token_input& in) -> result_t<T> {
			std::cout << "Trying " << rename << "..." << std::endl;
			auto res = fn(in);
			if (res.is_ok()) {
				std::cout << rename << " succeeds!" << std::endl;
				return res.get_ok();
			}
			else {
				auto& err = res.get_err();
				if (err.Fatal) {
					std::cout << rename << " fails FATALLY!" << std::endl;
				}
				else {
					std::cout << rename << " fails!" << std::endl;
				}
				return err;
			}
		});
#else
		return fn;
#endif
	}

	template <token_t TokenT>
	parser_t<token> terminal(ystr const& expect) {
		return [=](token_input& in) -> result_t<token> {
			auto result = in.head();
			if (result.is_ok()) {
				auto& tok = result.get_ok();
				if (tok.Type == TokenT) {
					token_input in2 = in.tail();
					return std::make_tuple(tok, in2);
				}
				else {
					return fail_info(
						in.get_index(), 
						parser_exp_tok_err(
							in.get_lexer().file(),
							expect,
							tok
						)
					);
				}
			}
			else {
				return fail_info(in.get_index(), result.get_err());
			}
		};
	}

	template <token_t TokT>
	parser_t<token> term() {
		return terminal<TokT>(token::static_desc(TokT));
	}
}
