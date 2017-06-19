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

#pragma once

#include <cassert>
#include "ast_expr.h"
#include "lexer.h"
#include "parselet.h"
#include "token.h"
#include "../common.h"

/**
 * A parselet set is a set of prefix and infix rules of a given node category.
 * For example the expression rules are in a parselet_set<AST_expr> container.
 */
template <typename T>
struct parselet_set {
	ymap<token_t, prefix_parselet<T>>	Prefix;	// Prefix rules
	ymap<token_t, infix_parselet<T>>	Infix;	// Infix rules

	/**
	 * Adds a prefix rule to the rule set.
	 * @param tt The token type that the parselet begins with.
	 * @param pp The parselet to apply.
	 */
	void add(token_t tt, prefix_parselet<T> pp) {
		assert(Prefix.find(tt) == Prefix.end() && "Prefix parselet already added!");
		Prefix.insert({ tt, pp });
	}

	/**
	 * Adds an infix rule to the rule set.
	 * @param tt The token type that the parselet begins with.
	 * @param pp The parselet to apply.
	 */
	void add(token_t tt, infix_parselet<T> pp) {
		assert(Infix.find(tt) == Infix.end() && "Infix parselet already added!");
		Infix.insert({ tt, pp });
	}
};

/**
 * The parser applies the rules of the rule sets based on the supported
 * token stream (usually the lexer).
 */
struct parser {
public:
	/**
	 * Creates a parser with a given lexer.
	 * @param lex The token stream to use while parsing.
	 */
	parser(lexer& lex);

public:
	/**
	 * Gets the file source that the lexer has.
	 * @return The file handle to the parsed source.
	 */
	file_hnd const& file() const;

	/**
	 * Peeks, but does not consume an upcoming token.
	 * @param delta The amount to peek forward, 0 by default.
	 * @return The peeked token.
	 */
	token const& peek(ysize delta = 0);

	/**
	 * Consumes the next token in the stream.
	 * @return The consumed token.
	 */
	token consume();

	/**
	 * Tries to parse an expression from the current tokens.
	 * @param min_prec The minimum precedence required to accept an operator.
	 * @preturn A pointer to an expression node. Null if none found.
	 * @see parse_set
	 */
	AST_expr* parse_expr(ysize min_prec = 0);

private:
	/**
	 * Parses based of a given rule set and precedence.
	 * @param set The rule set that can be applied.
	 * @param prec The minimum precedence required to accept an operator.
	 */
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
			assert(false && "The parselet should have thrown an error!");
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
				assert(false && "The parselet should have thrown an error!");
				return nullptr;
			}
		}
		// This should never happen
		assert(false && "Unreachable!");
		return nullptr;
	}

private:
	lexer&					m_Lexer;	// The token stream
	yvec<token>				m_Tokens;	// The peeked token buffer
	parselet_set<AST_expr>	m_Expr;		// The set of expression rules
};
