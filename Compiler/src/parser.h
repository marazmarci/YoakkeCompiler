/**
 * parser.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  Here is the parser for the language as well as a reference grammar.
 */

#pragma once

/******************************************************************************
*                                 GRAMMAR                                     *
******************************************************************************/
/**
 * expr:
 *       '(' expr ')'
 *     | IntLit
 *     | RealLit
 *     | Ident
 *     | expr '+' expr
 *     | expr '-' expr
 *     | expr '*' expr
 *     | expr '/' expr
 *     | expr '%' expr
 *     | expr '=' expr
 *     | expr '==' expr
 *     | expr '<>' expr
 *     | expr '>' expr
 *     | expr '<' expr
 *     | expr '>=' expr
 *     | expr '<=' expr
 *     | 'fn' [Ident] '(' ([Ident] ':' type)* ')' ['->' type] block_expr
 *     ;
 */

class parser {

};
