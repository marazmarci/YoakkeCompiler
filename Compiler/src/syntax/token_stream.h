#pragma once

#include "lexer.h"

struct token_stream {
public:
	token_stream(lexer& lex);

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
	 * Get the last consumed token.
	 * @return The last consumed token from the stream (Epsilon by default).
	 */
	token const& last() const;

	/**
	 * Consumes the next token in the stream.
	 * @return The consumed token.
	 */
	token consume();

	ysize token_count() const;

private:
	lexer&		m_Lexer;	// The token stream
	yvec<token>	m_Tokens;	// The peeked token buffer
	token		m_Last;		// The last consumed token
};
