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
 * A parser state represents the exact state of the parser to be able to
 * rollback to a previous position if needed.
 */
struct parser_state {
	lexer_state	LexState;	// The state of the lexer
	yvec<token> Tokens;		// The peek buffer
	token		Last;		// The last consumed token

	/**
	 * Creates a parser state from the lexer and internal parser state.
	 * @param lexs The state of the lexer.
	 * @param toks The token/peek buffer of the parser.
	 * @param last The last consumed token of the parser.
	 */
	parser_state(lexer_state const& lexs, yvec<token> const& toks, token const& last)
		: LexState(lexs), Tokens(toks), Last(last) {
	}
};

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
	 * Gets the current state of the parser.
	 * @return The current parser state.
	 */
	parser_state get_state() const;

	/**
	 * Sets the parser's state to rollback to a previous position.
	 * @param st The state to rollback to.
	 */
	void set_state(parser_state const& st);

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
	 * Get the last consumed token.
	 * @return The last consumed token from the stream (Epsilon by default).
	 */
	token const& last() const;

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

	/**
	 * Tries to parse a type descriptor from the current tokens.
	 * @param min_prec The minimum precedence required to accept an operator.
	 * @preturn A pointer to a type descriptor node. Null if none found.
	 * @see parse_set
	 */
	AST_ty* parse_ty(ysize min_prec = 0);

	/*
	 * Tries to parse a statement from the upcoming tokens.
	 * @return A pointer to a statement node. Null if none found.
	 */
	AST_stmt* parse_stmt();

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
		auto pre_parselet_it = set.Prefix.find(ahead.Type);
		if (pre_parselet_it == set.Prefix.end()) {
			// No such prefix operator
			return nullptr;
		}
		// Get the element for easy access
		auto& pre_parselet = pre_parselet_it->second;
		if (pre_parselet.Applyable &&
			!pre_parselet.Applyable(*this)) {
			// Cannot apply this
			return nullptr;
		}
		// Eat the token
		ahead = consume();
		// Parse using the prefix parselet
		T* left = pre_parselet.Func(*this, ahead);
		if (!left) {
			assert(false && "The parselet should have thrown an error!");
			return nullptr;
		}
		while (true) {
			ahead = peek();
			auto in_parselet_it = set.Infix.find(ahead.Type);
			if (in_parselet_it == set.Infix.end()) {
				// No such infix operator
				return left;
			}
			// Get the element for easy access
			auto& in_parselet = in_parselet_it->second;
			if (in_parselet.Applyable &&
				!in_parselet.Applyable(*this)) {
				// Cannot apply this
				return left;
			}
			if (prec >= in_parselet.Prec) {
				// Our precedence is greater, exit
				return left;
			}
			// Eat the token
			ahead = consume();
			// Parse with the parselet
			left = in_parselet.Func(*this, in_parselet.Prec, left, ahead);
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
	parselet_set<AST_ty>	m_Ty;		// The set of type rules
	token					m_Last;		// The last consumed token
};
