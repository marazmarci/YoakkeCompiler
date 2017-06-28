/**
 * parselet.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  A parselet is a kind of parser-combinator for precedence parsing.
 *  Here are all the parselets and combinators used for the language.
 */

#pragma once

#include <cassert>
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
struct AST_pat;

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
	using parselet_t = std::function<T* (parser&, ysize, T*, token const&)>;
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

namespace parselet {
	// TODO: A lot to clean up here

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
		return [=](parser& p, token const& beg) -> ResT {
			if (auto in = inner(p)) {
				if (auto end = term<EndTok>(p)) {
					return in;
				}
				else {
					error(p, enddesc, beg.Pos);
					return ResT();
				}
			}
			else {
				error(p, idesc);
				return ResT();
			}
		};
	}

	template <token_t EndTok, typename OtherT, typename Fn>
	auto make_enclose_or(Fn inner, const char* idesc, const char* enddesc) {
		using ResT = std::result_of_t<Fn(parser&)>;
		return [=](parser& p, token const& beg) -> ResT {
			if (auto in = inner(p)) {
				if (auto end = term<EndTok>(p)) {
					return in;
				}
				else {
					error(p, enddesc, beg.Pos);
					return ResT();
				}
			}
			else {
				if (auto end = term<EndTok>(p)) {
					return new OtherT(beg, unwrap(end));
				}
				else {
					error(p, idesc);
					return ResT();
				}
			}
		};
	}

	template <typename ElemT, token_t SEP, typename Fn>
	auto make_sep_seq(Fn get_elem, const char* elemdesc) {
		return [=](parser& p) {
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

	AST_pat* get_pat(parser& p, ysize prec);

	template <ysize PR>
	AST_pat* get_pat_p(parser& p) {
		return get_pat(p, PR);
	}

	AST_stmt* get_stmt(parser& p);

	AST_expr* get_func(parser& p, token const& beg);
	AST_ty* get_ty_not(parser& p);

	AST_body_expr* get_body(parser& p);

	AST_expr* get_if(parser& p, token const& beg);
	AST_expr* get_call(parser& p, ysize prec, AST_expr* left, token const& lpar);

	template <typename ResT, typename T, typename Fn>
	auto make_lassoc(Fn func, const char* rhs_desc) {
		return [=](parser& p, ysize prec, T* left, token const& begin) -> T* {
			T* right = func(p, prec);
			if (!right) {
				error(p, rhs_desc);
				return nullptr;
			}
			return new ResT(begin, left, right);
		};
	}

	template <typename ResT, typename T, typename Fn>
	auto make_rassoc(Fn func, const char* rhs_desc) {
		return [=](parser& p, ysize prec, T* left, token const& begin) -> T* {
			assert(prec > 0 && "Right associative operator must have a precedence greater than 0!");
			T* right = func(p, prec - 1);
			if (!right) {
				error(p, rhs_desc);
				return nullptr;
			}
			return new ResT(begin, left, right);
		};
	}

	template <typename NodT, typename ElemT, token_t SEP, typename Fn>
	auto make_from_seq(Fn get_elem, const char* elemdesc) {
		return [=](parser& p, ysize prec, ElemT* first, token const& beg) -> NodT* {
			yvec<ElemT*> elems{ first };
			if (auto el = get_elem(p, prec + 1)) {
				elems.push_back(unwrap(el));
				while (term<SEP>(p)) {
					auto el2 = get_elem(p, prec + 1);
					if (!el2) {
						error(p, elemdesc);
						return nullptr;
					}
					elems.push_back(unwrap(el2));
				}
				return new NodT(elems);
			}
			else {
				error(p, elemdesc);
				return nullptr;
			}
		};
	}
}
