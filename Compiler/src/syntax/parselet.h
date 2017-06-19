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
struct file_hnd;

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

/**
 * This exception is thrown when the parser expects a different input from
 * the one that was provided.
 */
struct parser_expect_exception {
	file_hnd const& File;	// The file the error occured in
	yopt<interval> BegPos;	// An optional beginning position
	interval Pos;			// The position of the error
	ystr Expected;			// What the expected thing was
	ystr Got;				// What we have got instead

	/**
	 * Creates a new expect error with the given position and expectation.
	 * @param file The file the error occured in.
	 * @param pos The position of the error.
	 * @param expect What was expected.
	 * @param got What we have got instead of the expected.
	 * @param beg Optional beginning position of the error.
	 */
	parser_expect_exception(file_hnd const& file, interval const& pos,
		ystr const& expect, ystr const& got, yopt<interval> beg = {})
		: File(file), Pos(pos), Expected(expect), Got(got), BegPos(beg) {
	}
};

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

	/**
	 * Creates an enclosing parselet that encloses a subexpression
	 * with two tokens.
	 * @param func The parselet for the subexpression.
	 * @param right_t The token type of the closing token.
	 * @param in_desc A description of the subexpression (for error reporting).
	 * @param end_desc A description of the closing token (for error reporting).
	 */
	template <typename T, typename Fn>
	inline prefix_parselet<T> enclose(Fn func, token_t right_t, ystr const& in_desc, ystr const& end_desc) {
		return [&func, right_t, in_desc, end_desc](parser& p, token const& lhs) -> T* {
			T* sub = func(p, 0);
			if (!sub) {
				token const& ahead = p.peek();
				throw parser_expect_exception(p.file(), ahead.Pos,
					in_desc, ahead.fmt());
				return nullptr;
			}
			if (p.peek().Type != right_t) {
				token const& ahead = p.peek();
				throw parser_expect_exception(p.file(), ahead.Pos,
					end_desc, ahead.fmt(), lhs.Pos);
				return nullptr;
			}
			token rhs = p.consume();
			// Modify position (include enclosing tokens)
			sub->Pos = interval(lhs.Pos, rhs.Pos);
			return sub;
		};
	}
}

namespace infix {
	/**
	 * Creates a left-associative infix parselet.
	 * @param The precedence of the parselet.
	 * @param func The parselet for the right-hand side.
	 * @param rhs_desc What is expected on the right-hand side 
	 * (for error reporting).
	 * @return The infix parselet.
	 * @throw parser_expect_exception When the right-hand side parsing failed.
	 */
	template <typename T, typename ResT, typename Fn>
	inline infix_parselet<T> lassoc(ysize prec, Fn func, ystr const& rhs_desc) {
		return {
			prec,
			[prec, &func, rhs_desc](parser& p, T* left, token const& begin) -> T* {
				T* right = func(p, prec);
				if (!right) {
					token const& ahead = p.peek();
					throw parser_expect_exception(p.file(), ahead.Pos,
						rhs_desc, ahead.fmt());
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
	 * @param rhs_desc What is expected on the right-hand side
	 * (for error reporting).
	 * @return The infix parselet.
	 * @throw parser_expect_exception When the right-hand side parsing failed.
	 */
	template <typename T, typename ResT, typename Fn>
	inline infix_parselet<T> rassoc(ysize prec, Fn func, ystr const& rhs_desc) {
		assert(prec > 0 && "Right associative operator must have a precedence greater than 0!");
		return {
			prec,
			[prec, &func, &rhs_desc](parser& p, T* left, token const& begin) -> T* {
				T* right = func(p, prec - 1);
				if (!right) {
					token const& ahead = p.peek();
					throw parser_expect_exception(p.file(), ahead.Pos,
						rhs_desc, ahead.fmt());
					return nullptr;
				}
				return new ResT(begin, left, right);
			}
		};
	}
}
