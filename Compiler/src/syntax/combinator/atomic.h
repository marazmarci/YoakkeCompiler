#pragma once

#include <functional>
#include "../lexer.h"
#include "../../common.h"

template <typename T>
struct parser_monad {
public:
	closure_t parse;

public:
	parser_monad(closure_t closure)
		: parse(closure) {
	}

public:
	using closure_t = std::function<yopt<T>(lexer& lex)>;
};

namespace combinator {
	template <typename T>
	parser_monad<T> pure(T res) {
		return parser_monad<T>(
			[=]() -> yopt<T> { return res; }
		);
	}

	template <typename T>
	parser_monad<T> empty() {
		return parser_monad<T>(
			[]() -> yopt<T> { return {}; }
		);
	}

	template <typename A, typename B>
	parser_monad<B> operator>>(parser_monad<A> p, std::function<parser_monad<B>(A)> f) {
		return parser_monad<B>(
			[=](lexer& lex) -> yopt<T> {
				if (auto result = p.parse(lex)) {
					return f(result).parse(lex);
				}
				else {
					return {};
				}
			}
		);
	}

	template <typename A, typename B>
	parser_monad<B> operator^(std::function<B(A)> f, parser_monad<A> p) {
		return p >> [=](auto a) { return pure(f(a)); };
	}
}
