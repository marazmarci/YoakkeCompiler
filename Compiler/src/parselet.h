/**
 * parselet.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  A parselet is a kind of parser-combinator for precedence parsing.
 */

#pragma once

#include <functional>
#include <type_traits>
#include "common.h"

struct token;
class parser;

/**
 * A prefix parselet is just a function that receives a beginning token and
 * the parser that the function was called from.
 * The template parameter T is the pointer type that the function returns.
 */
template <typename T>
using prefix_parselet = std::function<T* (parser&, token const&)>;

/**
 * An infix parselet is a pair of precedence and a function. The function
 * receives the parser the function was called from, the left-hand side of
 * the parsed piece and the beginning of the current one.
 * The template parameter T is the pointer type that the function receives
 * and returns.
 */
template <typename T>
using infix_parselet = ypair<
	ysize, 
	std::function<T* (parser&, T*, token const&)>
>;

// TODO: commenting

namespace prefix {
	template <typename T>
	inline prefix_parselet<T> pass() {
		return [](parser&, token const& t) {
			return new T(t);
		};
	}
}

namespace infix {
	template <typename T, typename ResT, typename Fn>
	inline infix_parselet<T> lassoc(ysize prec, Fn func) {
		return {
			prec,
			[prec, &func](parser& p, T* left, token const& begin) -> T* {
				T* right = func(p, prec);
				if (!right) {
					// TODO: error
					return nullptr;
				}
				return new ResT(begin, left, right);
			}
		};
	}

	template <typename T, typename ResT, typename Fn>
	inline infix_parselet<T> rassoc(ysize prec, Fn func) {
		assert(prec > 0 && "Right associative operator must have a precedence greater than 0!");
		return {
			prec,
			[prec, &func](parser& p, T* left, token const& begin) -> T* {
				T* right = func(p, prec - 1);
				if (!right) {
					// TODO: error
					return nullptr;
				}
				return new ResT(begin, left, right);
			}
		};
	}
}
