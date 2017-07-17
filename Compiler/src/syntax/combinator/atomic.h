#pragma once

#include <functional>
#include "../lexer.h"
#include "../token_state.h"
#include "../../common.h"

template <typename T>
struct parser_monad {
public:
	using closure_t = std::function<yopt<ypair<T, token_state>>(token_state lex)>;

public:
	closure_t parse;

public:
	parser_monad(closure_t closure)
		: parse(closure) {
	}
};

namespace combinator {
	auto atom(token_t tt) {
		return [=](token_state lex) -> yopt<ypair<token, token_state>> {
			if (lex.head().Type == tt) {
				return ypair<token, token_state>{ lex.head(), lex.tail() };
			}
			else {
				return {};
			}
		};
	}

	template <typename T>
	T identity(T t) {
		return t;
	}

	template <typename A, typename B>
	auto constant(A a) {
		return [=](auto ignore) { return a; };
	}

	template <typename T>
	parser_monad<T> pure(T res) {
		return parser_monad<T>(
			[=](auto param) -> yopt<ypair<T, token_state>> { 
				return ypair<T, token_state>{ res, param };
			}
		);
	}

	template <typename T>
	parser_monad<T> empty() {
		return parser_monad<T>(
			[]() -> yopt<T> { return {}; }
		);
	}

	// TODO from here
	template <typename A, typename B>
	parser_monad<B> operator>>(parser_monad<A> p, std::function<parser_monad<B>(A)> f) {
		return parser_monad<B>(
			[=](token_state& lex) -> yopt<B> {
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

	template <typename A>
	parser_monad<A> operator|(parser_monad<A> p, parser_monad<A> q) {
		return parser_monad<A>(
			[=](token_state& lex) -> yopt<A> {
				if (auto res = p.parse(lex)) {
					return res;
				}
				else {
					return q.parse(lex);
				}
			}
		);
	}

	template <typename A, typename B>
	parser_monad<B> operator*(parser_monad<std::function<B(A)>> p, parser_monad<A> q) {
		return p >> [=](auto f) { return f ^ q; }
	}

	template <typename A, typename B>
	parser_monad<A> operator<(parser_monad<A> p, parser_monad<B> q) {
		return constant ^ p * q;
	}

	template <typename A, typename B>
	parser_monad<B> operator>(parser_monad<A> p, parser_monad<B> q) {
		return constant(identity) ^ p * q;
	}

	template <typename A>
	parser_monad<yvec<A>> many1(parser_monad<A> p);

	template <typename A>
	parser_monad<yvec<A>> many(parser_monad<A> p) {
		return many1(p) | pure(yvec<A>{});
	}

	template <typename A>
	parser_monad<yvec<A>> many1(parser_monad<A> p) {
		return constant ^ p * many(p);
	}

	template <typename A>
	parser_monad<unit> skip_many1(parser_monad<A> p);

	template <typename A>
	parser_monad<unit> skip_many(parser_monad<A> p) {
		return skip_many1(p) | pure(std::make_tuple());
	}

	template <typename A>
	parser_monad<unit> skip_many1(parser_monad<A> p) {
		return p > skip_many(p);
	}
}
