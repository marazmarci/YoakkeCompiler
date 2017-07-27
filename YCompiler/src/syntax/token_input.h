/**
 * token_input.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  This structure provides a stateless token stream.
 */

#pragma once

#include "../common.h"

// TODO: const-ing, mutable

struct lexer;
struct token;

struct token_input {
public:
	/**
	 * Creates a token input at the beginning of the lexer.
	 * @param lex The lexer to read from.
	 */
	token_input(lexer& lex);

	/**
	 * Implements the assignment operator for the type for mutability.
	 * @param other The token input to be copied.
	 * @return An immutable reference to this object.
	 */
	token_input const& operator=(token_input const& other);

public:
	/**
	 * Returns the token at the position this instance points at.
	 * @return The pointed token or a lexer error.
	 */
	yresult<token, lexer_err> head();
	
	/**
	 * Returns the token input proceeding this one.
	 * @return The token input containing the rest of the tokens.
	 */
	token_input tail();

	/**
	 * Returns the lexer used by this input wrapper.
	 * @return The underlying lexer.
	 */
	lexer const& get_lexer() const;

	/**
	 * Returns the index of this input wrapper.
	 * @return The token index of the token pointer.
	 */
	ysize get_index() const;

private:
	/**
	 * Internally constructs a token input at the given position.
	 * Used by tail().
	 * @see tail().
	 * @param lex The lexer to get the input from.
	 * @param idx The position in the lexer.
	 */
	token_input(lexer& lex, ysize idx);

private:
	lexer& m_Lexer;
	ysize m_Index;
};
