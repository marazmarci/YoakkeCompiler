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
#include "../common.h"

struct token;
struct parser;

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

namespace prefix {
	/**
	 * Creates a passing parselet that constructs a node from a single token.
	 * @return The parselet.
	 */
	template <typename T>
	inline prefix_parselet<T> pass() {
		return [](parser&, token const& t) {
			return new T(t);
		};
	}
}

namespace infix {
	/**
	 * Creates a left-associative infix parselet.
	 * @param The precedence of the parselet.
	 * @param func The parselet for the right-hand side.
	 * @return The infix parselet.
	 */
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

	/**
	 * Creates a right-associative infix parselet.
	 * @param The precedence of the parselet.
	 * @param func The parselet for the right-hand side.
	 * @return The infix parselet.
	 */
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
