/**
 * parselet.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  A parselet is a kind of parser-combinator for precedence parsing.
 */

#pragma once

#include <functional>
#include "common.h"

struct token;
class parser;

/**
 * A prefix parselet is just a function that receives a beginning token and
 * the parser that the function was called from.
 * The template parameter T is the pointer type that the function returns.
 */
template <typename T>
using prefix_parselet = std::function<ysptr<T>(parser&, token const&)>;

/**
 * An infix parselet is a pair of precedence and a function. The function
 * receives the parser the function was called from, the left-hand side of
 * the parsed piece and the beginning of the current one.
 * The template parameter T is the pointer type that the function receives
 * and returns.
 */
template <typename T>
using infix_parselet = ypair<
	ysize, 
	std::function<ysptr<T>(parser&, ysptr<T>, token const&)>
>;
