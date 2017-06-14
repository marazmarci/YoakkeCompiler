/**
 * token.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  A token is the result of the lexical analysis, every token represents
 *  an atomic part of the source like an identifier, number, braces and so on.
 */

#pragma once

#include "common.h"
#include "position.h"

/**
 * The types that a token can be.
 */
enum class token_t {
	Epsilon,	// Empty token
	EndOfFile,	// End of file
	Ident,		// Identifier			[A-Za-z_][A-Za-z_0-9]*
	IntLit,		// Integer literal		[0-9]+
	RealLit,	// Real literal			[0-9]+\.[0-9]+
	LParen,		// Left parenthesis		'('
	RParen,		// Right parenthesis	')'
	LBrace,		// Left brace			'{'
	RBrace,		// Right brace			'}'
	LBracket,	// Left bracket			'['
	Rbracket,	// Right bracket		']'
	Add,		// Plus sign			'+'
	Sub,		// Minus sign			'-'
	Mul,		// Star sign			'*'
	Div,		// Slash sign			'/'
	Mod,		// Percent sign			'%'
	Asgn,		// Equals sign			'='
	Eq,			// Double equals sign	'=='
	Neq,		// Less-greater sign	'<>'
	Greater,	// Greater sign			'>'
	Less,		// Less sign			'<'
	GrEq,		// Greater-or-eq sign	'>='
	LeEq,		// Greater-or-eq sign	'<='
	Dot,		// Dot					'.'
	Colon,		// Colon				':'
	Semicol,	// Semicolon			';'
	Arrow,		// Arrow sign			'->'
};

/**
 * The actual token structure, created by the lexer and read by the parser.
 * The Value filed is only used if nesecarry (number, identifier, string, ...)
 */
struct token {
	interval	Pos;	// Position in file
	token_t		Type;	// Category
	ystr		Value;	// Content

	/**
	 * Creates a token from a position, type and a value.
	 * @param pos The position of the token in the file.
	 * @param type The type of the token.
	 * @param val The contents of the token (optional).
	 */
	token(interval const& pos, token_t type, ystr const& val = "")
		: Pos(pos), Type(type), Value(val) {
	}
};
