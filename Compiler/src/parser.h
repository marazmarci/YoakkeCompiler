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
 * program:
 *       (glob_stmt)*
 *     ;
 *
 * stmt:
 *       glob_stmt
 *     ;
 *
 * expr:
 *       '(' [expr] ')'
 *     | IntLit
 *     | RealLit
 *     | Ident
 *     | '^' expr
 *     | expr '^'
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
 *     | expr (',' expr)*
 *     | 'fn' [Ident] '(' ([Ident] ':' type)* ')' ['->' type] block_expr
 *     | 'let' pattern [':' type] ['=' expr]
 *     | braced_expr
 *     ;
 *
 * braced_expr:
 *       block_expr
 *     | if_expr
 *     | while_expr
 *     ;
 *
 * block_expr:
 *       '{' (stmt)* '}'
 *     ;
 */

class parser {

};
