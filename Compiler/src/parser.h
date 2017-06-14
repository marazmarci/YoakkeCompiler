/**
 * parser.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  Here is the parser for the language as well as a reference grammar.
 */

/******************************************************************************
*                                 GRAMMAR                                     *
******************************************************************************/
/**
 * TODO
 */

// TODO: comment

#pragma once

#include <cassert>
#include "ast_expr.h"
#include "common.h"
#include "lexer.h"
#include "parselet.h"
#include "token.h"

template <typename T>
struct parselet_set {
	ymap<token_t, prefix_parselet<T>>	Prefix;
	ymap<token_t, infix_parselet<T>>	Infix;

	void add(token_t tt, prefix_parselet<T> pp) {
		assert(Prefix.find(tt) == Prefix.end() && "Prefix parselet already added!");
		Prefix.insert({ tt, pp });
	}

	void add(token_t tt, infix_parselet<T> pp) {
		assert(Infix.find(tt) == Infix.end() && "Infix parselet already added!");
		Infix.insert({ tt, pp });
	}
};

class parser {
public:
	parser(lexer& lex);

public:
	AST_expr* parse_expr(ysize min_prec = 0);

private:
	template <typename T>
	T* parse_set(parselet_set<T>& set, ysize prec) {
		// Look what token is coming
		token ahead = peek();
		// Check if we have a prefix parselet for that
		auto pre_parselet = set.Prefix.find(ahead.Type);
		if (pre_parselet == set.Prefix.end()) {
			// No such prefix operator
			return nullptr;
		}
		// Eat the token
		ahead = consume();
		// Parse using the prefix parselet
		T* left = pre_parselet->second(*this, ahead);
		if (!left) {
			// TODO: error?
			return nullptr;
		}
		while (true) {
			ahead = peek();
			auto in_parselet = set.Infix.find(ahead.Type);
			if (in_parselet == set.Infix.end()) {
				// No such infix operator
				return left;
			}
			if (prec >= in_parselet->second.first) {
				// Our precedence is greater, exit
				return left;
			}
			// Eat the token
			ahead = consume();
			// Parse with the parselet
			left = in_parselet->second.second(*this, left, ahead);
			if (!left) {
				// TODO: error?
				return nullptr;
			}
		}
		// This should never happen
		assert(false && "Unreachable!");
		return nullptr;
	}

private:
	token const& peek(ysize delta = 0);
	token consume();

private:
	lexer&					m_Lexer;
	yvec<token>				m_Tokens;
	parselet_set<AST_expr>	m_Expr;
};
