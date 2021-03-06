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

#include "position.h"
#include "token.h"
#include "../common.h"
#include "../io/file_hnd.h"

/**
 * This error occurs when there is something that needs to be closed
 * tokenwise, but the end of file was reached without closing it. This could
 * be an unmatched nested comment, or missing aphostrophe at the end of a
 * string literal.
 */
struct lexer_eof_err {
	file_hnd const& File;	// The file where the error happened
	interval		Start;	// Where was the beginning of the token
	interval		End;	// Where was the last visible character
	ystr			Msg;	// What was not escaped
	yopt<ystr>		Note;	// An optional note for the user

	/**
	 * Creates an unexpected EOF error at the given positions.
	 * @param file The source file.
	 * @param start The beginning of the token.
	 * @param end The last visible character's place in the file.
	 * @param msg A message telling what was unescaped.
	 * @param note An optional note for the user for specific help.
	 */
	lexer_eof_err(file_hnd const& file,
		interval const& start, interval const& end,
		ystr const& msg, yopt<ystr> note = {});
};

/**
 * This error occurs when there is something that needs to be closed
 * tokenwise, but the end of line was reached without closing it. This could
 * be an unmatched string quote.
 */
struct lexer_eol_err {
	file_hnd const& File;	// The file where the error happened
	interval		Start;	// Where was the beginning of the token
	interval		End;	// Where was the last visible character
	ystr			Msg;	// What was not escaped
	yopt<ystr>		Note;	// An optional note for the user

	/**
	 * Creates an unexpected EOF error at the given positions.
	 * @param file The source file.
	 * @param start The beginning of the token.
	 * @param end The last visible character's place in the file.
	 * @param msg A message telling what was unescaped.
	 * @param note An optional note for the user for specific help.
	 */
	lexer_eol_err(file_hnd const& file,
		interval const& start, interval const& end,
		ystr const& msg, yopt<ystr> note = {});
};

/**
 * This occurs when an unknown token is encountered.
 */
struct lexer_unk_tok_err {
public:
	file_hnd const& File;	// The file where the error happened
	interval		Pos;	// Where the error happened
	ystr			Tok;	// The value of the token

public:
	/**
	 * Creates an unknown token error with a given position.
	 * @param file The file the error happened in.
	 * @param pos The position of the unknown token.
	 * @param val The unknown token literal itself.
	 */
	lexer_unk_tok_err(file_hnd const& file, interval const& pos, ystr const& val);
};

/**
 * This is the lexer error type that can occur.
 */
using lexer_err = yvar<lexer_eof_err, lexer_eol_err, lexer_unk_tok_err>;

/**
 * The lexer state can be used to go back to a previous state if needed.
 * Mainly used by the parser.
 * @see lexer
 */
using lexer_state = ytup<point, yvec<token>>;

/**
 * A lexer instance analyses a single file from a handle, outputting tokens
 * with positional information. The lexer has an internal state that can be
 * used to restore a previous state if needed.
 */
struct lexer {
public:
	// Illegal operations
	lexer(lexer const&) = delete;
	lexer& operator=(lexer const&) = delete;

	/**
	 * Creates a new lexer from a file handle.
	 * @param src The file handle to tokenize.
	 */
	lexer(file_hnd const& src);

public:
	/**
	 * Gets the file source that the lexer has.
	 * @return The file handle to the lexed source.
	 */
	file_hnd const& file() const;

	/**
	 * Get the current state of the lexer.
	 * @return A lexer_state of the lexer.
	 * @see lexer_state
	 */
	lexer_state const& get_state() const;

	/**
	 * Get the current state of the lexer.
	 * @return A lexer_state of the lexer.
	 * @see lexer_state
	 */
	lexer_state& get_state();

	/**
	 * Sets the current lexer state to a given one.
	 * @param state The lexer state to set the lexer to.
	 */
	void set_state(lexer_state const& state);

	/**
	 * Peeks a number of tokens ahead.
	 * @param delta How much to peek ahead (0 by default).
	 * @return A token delta tokens away from the starting point or a lexer error.
	 */
	yresult<token, lexer_err> peek(ysize delta = 0);

	/**
	 * Consumes a token from the token buffer.
	 * @return The consumed token or a lexer error.
	 */
	yresult<token, lexer_err> consume();

	/**
	 * Consumes and gets the next token from the file.
	 * @return The next token in the file. If there is none, a token withe
	 * type EndOfFile is returned.
	 */
	yresult<token, lexer_err> next();

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
	point			m_Last;		// The last visible character's position
	const char*		m_Ptr;		// A pointer in the source for easy lexing

	ymap<ystr, token_t>	m_Symbols;	// Tokens with special characters (greedy)
	ymap<ystr, token_t> m_Keywords;	// Identifier-based tokens (non-greedy)
};
