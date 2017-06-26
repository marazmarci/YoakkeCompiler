/**
* parselet_common.h
* Created by: Peter Lenkefi
* All rights reserved.
*
* Description:
*  These are the common parselets for helping building the others.
*/

#pragma once

#include "token.h"
#include "parser.h"

namespace parselet_common {
	inline void parse_error(parser& p, ystr const& exp) {
		auto ahead = p.peek();
		throw parser_expect_exception(p.file(), ahead.Pos, exp, ahead.fmt());
	}

	inline void parse_error(parser& p, ystr const& exp, interval const& prev) {
		auto ahead = p.peek();
		throw parser_expect_exception(p.file(), ahead.Pos, exp, ahead.fmt(), prev);
	}

	template <typename Fn>
	inline auto opt(Fn f) {
		return [f](parser& p) {
			return f(p);
		};
	}

	template <typename T, typename Fn>
	inline auto req(Fn f, ystr const& desc) {
		return [f, desc](parser& p) {
			if (auto res = f(p)) {
				return res;
			}
			parse_error(p, desc);
			return T();
		};
	}

	template <typename T, typename Fn>
	inline auto req(Fn f, ystr const& desc, interval const& prevp) {
		return [f, desc, prevp](parser& p) {
			if (auto res = f(p)) {
				return res;
			}
			parse_error(p, desc, prevp);
			return T();
		};
	}

	template <token_t TT>
	inline yopt<token> get_token(parser& p) {
		if (p.peek().Type == TT) {
			return p.consume();
		}
		return {};
	}

	template <typename T, typename Fn1, typename Fn2, typename Fn3>
	inline auto enclose(Fn1 lhs, Fn2 inner, Fn3 rhs, 
		ystr const& inner_desc = "", ystr const& end_desc = "") {
		return [lhs, inner, rhs, &inner_desc, &end_desc](parser& p) {
			if (auto left = opt(lhs)(p)) {
				auto exp	= req<T>(inner, inner_desc)(p);
				auto right	= req<T>(rhs, end_desc, left->Pos)(p);
				return exp;
			}
			return T();
		};
	}

	yopt<token> get_label(parser& p);
}
