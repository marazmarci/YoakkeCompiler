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
	token_input(lexer& lex);

public:
	token& head();
	token_input tail();

private:
	token_input(lexer& lex, ysize idx);

private:
	lexer& m_Lexer;
	const ysize m_Index;
};
