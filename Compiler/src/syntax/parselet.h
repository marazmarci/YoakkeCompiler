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
#include "token.h"
#include "position.h"
#include "../common.h"

struct token;
struct parser;
struct file_hnd;
struct AST_expr;
struct AST_ty;
struct AST_stmt;
struct AST_body_expr;

/**
 * A prefix parselet is just a function that receives a beginning token and
 * the parser that the function was called from.
 * The template parameter T is the pointer type that the function returns.
 */
template <typename T>
struct prefix_parselet {
public:
	// Just for shorter types
	using parselet_t = std::function<T* (parser&, token const&)>;
	using apply_t = std::function<bool(parser&)>;

public:
	parselet_t	Func;		// Parser function
	apply_t		Applyable;	// Can this be applied?

	/**
	 * Creates a prefix parselet with the given function.
	 * @param func The parser function.
	 * @param apply A predicate telling if the parselet is applyable (optional).
	 */
	prefix_parselet(parselet_t func, apply_t apply = nullptr)
		: Func(func), Applyable(apply) {
	}
};

/**
 * An infix parselet consist of a precedence and a function. The function
 * receives the parser the function was called from, the left-hand side of
 * the parsed piece and the beginning of the current one.
 * The template parameter T is the pointer type that the function receives
 * and returns.
 */
template <typename T>
struct infix_parselet {
public:
	// Just for shorter types
	using parselet_t = std::function<T* (parser&, T*, token const&)>;
	using apply_t = std::function<bool(parser&)>;

public:
	ysize		Prec;		// The precedence of the parselet
	parselet_t	Func;		// Parser function
	apply_t		Applyable;	// Can this be applied?

	/**
	 * Creates an infix parselet with the given precedence and function.
	 * @param prec The precedence this parser can be applied.
	 * @param func The parser function.
	 * @param apply A predicate telling if the parselet is applyable (optional).
	 */
	infix_parselet(ysize prec, parselet_t func, apply_t apply = nullptr)
		: Prec(prec), Func(func), Applyable(apply) {
	}
};

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
	template <typename U, typename T>
	inline prefix_parselet<U> pass() {
		return 
		prefix_parselet<U>(
			[](parser&, token const& t) -> U* {
				return new T(t);
			}
		);
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
		return 
		prefix_parselet<T>(
		[&func, right_t, in_desc, end_desc](parser& p, token const& lhs) -> T* {
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
		});
	}
}

namespace infix {
	/**
	 * Creates a left-associative infix parselet.
	 * @param The precedence of the parselet.
	 * @param func The parselet for the right-hand side.
	 * @param rhs_desc What is expected on the right-hand side 
	 * (for error reporting).
	 * @param apply Optional predicate for when can be applied.
	 * @return The infix parselet.
	 * @throw parser_expect_exception When the right-hand side parsing failed.
	 */
	template <typename T, typename ResT, typename Fn>
	inline infix_parselet<T> 
	lassoc(	ysize prec, Fn func, ystr const& rhs_desc, 
			typename infix_parselet<T>::apply_t apply = nullptr) {
		return infix_parselet<T>(
			prec,
			[prec, func, rhs_desc](parser& p, T* left, token const& begin) -> T* {
				T* right = func(p, prec);
				if (!right) {
					token const& ahead = p.peek();
					throw parser_expect_exception(p.file(), ahead.Pos,
						rhs_desc, ahead.fmt());
					return nullptr;
				}
				return new ResT(begin, left, right);
			},
			apply
		);
	}

	/**
	 * Creates a right-associative infix parselet.
	 * @param The precedence of the parselet.
	 * @param func The parselet for the right-hand side.
	 * @param rhs_desc What is expected on the right-hand side
	 * (for error reporting).
	 * @param apply Optional predicate for when can be applied.
	 * @return The infix parselet.
	 * @throw parser_expect_exception When the right-hand side parsing failed.
	 */
	template <typename T, typename ResT, typename Fn>
	inline infix_parselet<T> 
	rassoc(	ysize prec, Fn func, ystr const& rhs_desc,
			typename infix_parselet<T>::apply_t apply = nullptr) {
		assert(prec > 0 && "Right associative operator must have a precedence greater than 0!");
		return infix_parselet<T>(
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
			},
			apply
		);
	}
}

namespace parselet {
	void error(parser& p, ystr const& what);
	void error(parser& p, ystr const& what, interval const& from);

	template <typename T>
	auto& unwrap(T& val) {
		return val;
	}

	template <typename T>
	auto& unwrap(yopt<T>& val) {
		return val.value();
	}

	template <token_t TT>
	yopt<token> term(parser& p) {
		if (p.peek().Type == TT) {
			return p.consume();
		}
		return {};
	}

	template <typename T>
	T* from_term(parser& p, token const& tok) {
		return new T(tok);
	}

	template <token_t EndTok, typename Fn>
	auto make_enclose(Fn inner, const char* idesc, const char* enddesc) {
		using ResT = std::result_of_t<Fn(parser&)>;
		return [&](parser& p, token const& beg) -> ResT {
			if (auto in = inner(p)) {
				if (auto end = term<EndTok>(p)) {
					return in;
				}
				else {
					error(p, idesc, beg.Pos);
					return ResT();
				}
			}
			else {
				error(p, idesc);
				return ResT();
			}
		};
	}

	template <typename ElemT, token_t SEP, typename Fn>
	auto make_sep_seq(Fn get_elem, const char* elemdesc) {
		return [&](parser& p) {
			yvec<ElemT> elems;
			if (auto el = get_elem(p)) {
				elems.push_back(unwrap(el));
				while (term<SEP>(p)) {
					auto el2 = get_elem(p);
					if (!el2) {
						error(p, elemdesc);
						return yvec<ElemT>{};
					}
					elems.push_back(unwrap(el2));
				}
				return elems;
			}
			return elems;
		};
	}

	yopt<token> get_label(parser& p);

	AST_expr* get_expr(parser& p, ysize prec);

	template <ysize PR>
	AST_expr* get_expr_p(parser& p) {
		return get_expr(p, PR);
	}

	AST_ty* get_ty(parser& p, ysize prec);

	template <ysize PR>
	AST_ty* get_ty_p(parser& p) {
		return get_ty(p, PR);
	}

	AST_stmt* get_stmt(parser& p);

	AST_expr* get_func(parser& p, token const& beg);
	AST_ty* get_ty_not(parser& p);

	AST_body_expr* get_body(parser& p);
}
