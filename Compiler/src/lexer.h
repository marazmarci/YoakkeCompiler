/**
 * lexer.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  The lexical analyzer that splits up a character stream into series
 *  of tokens. Used by the parser.
 */

#pragma once

#include "common.h"
#include "file_hnd.h"
#include "position.h"
#include "token.h"

/**
 * The lexer state can be used to go back to a previous state if needed.
 * Mainly used by the parser.
 * @see lexer
 */
using lexer_state = point;

/**
 * A lexer instance analyses a single file from a handle, outputting tokens
 * with positional information. The lexer has an internal state that can be
 * used to restore a previous state if needed.
 */
class lexer {
public:
	/**
	 * Creates a new lexer from a file handle.
	 * @param src The file handle to tokenize.
	 */
	lexer(file_hnd const& src);

public:
	/**
	 * Get the current state of the lexer.
	 * @return A lexer state of the lexer.
	 * @see lexer_state
	 */
	lexer_state get_state() const;

	/**
	 * Sets the current lexer state to a given one.
	 * @param state The lexer state to set the lexer to.
	 */
	void set_state(lexer_state const& state);

	/**
	 * Consumes and gets the next token from the file.
	 * @return The next token in the file. If there is none, a token withe
	 * type EndOfFile is returned.
	 * @throw TODO
	 */
	token next();

	/**
	 * Returns wether there are remaining tokens in the file or not. The
	 * EndOfFile is not qualified as a remaining token in this case. If
	 * there are only whitespaces remaining, the return value will still be
	 * true!
	 * @return True, if the pointer has not reached the EndOfFile.
	 */
	bool has_next() const;

private:
	/**
	 * Returns id we have reached the end of file.
	 * @return True, if end of file.
	 */
	bool end() const;

	/**
	 * Advances the state of the lexer by some amount (Jumps N chars forward).
	 * @param delta The amount to consume (1 by default)
	 */
	void advance(ysize delta = 1);

	/**
	 * Adds a symbol to the symbols list. Do not use ymap::insert!
	 * Use this instead.
	 * @param sym The symbol's string.
	 * @param ty The symbol's type.
	 */
	void add_symbol(ystr const& sym, token_t ty);

	/**
	* Adds a keyword to the keywords list. Do not use ymap::insert!
	* Use this instead.
	* @param kw The keyword.
	* @param ty The keyword's type.
	*/
	void add_keyword(ystr const& kw, token_t ty);

private:
	file_hnd const& m_File;		// The handle for the tokenized file
	lexer_state		m_State;	// The current state of the lexer
	const char*		m_Ptr;		// A pointer in the source for easy lexing

	ymap<ystr, token_t>	m_Symbols;	// Tokens with special characters (greedy)
	ymap<ystr, token_t> m_Keywords;	// Identifier-based tokens (non-greedy)
};
