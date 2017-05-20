#pragma once

#include <variant>
#include <boost\preprocessor\variadic\size.hpp>
#include "../common.h"

namespace yk {
	template <typename TF, typename... TFs>
	struct overload_set : TF, overload_set<TFs...> {
		using TF::operator();
		using overload_set<TFs...>::operator();

		template <typename TFFwd, typename... TRest>
		overload_set(TFFwd&& f, TRest&&... rest)
			: TF{ std::forward<TFFwd>(f) },
			overload_set<TFs...>{std::forward<TRest>(rest)...} {
		}
	};

	template <typename TF>
	struct overload_set<TF> : TF {
		using TF::operator();

		template <typename TFFwd>
		overload_set(TFFwd&& f)
			: TF{ std::forward<TFFwd>(f) } {
		}
	};

	template <typename... TFs>
	auto overload(TFs&&... fs) {
		return overload_set<std::remove_reference_t<TFs>...>(
			std::forward<TFs>(fs)...
			);
	}

	template <typename... TVariants>
	constexpr auto match(TVariants&&... vs) {
		return[&vs...](auto&&... fs) -> decltype(auto) {
			auto visitor = overload(std::forward<decltype(fs)>(fs)...);
			return std::visit(visitor, std::forward<TVariants>(vs)...);
		};
	}

	template <typename Tup, typename F, ysize... I>
	constexpr decltype(auto) bind_impl(Tup& tup, F&& f, std::index_sequence<I...>) {
		return std::invoke(std::forward<F>(f), std::get<I>(tup)...);
	}

	template <typename Tup, typename F>
	constexpr decltype(auto) bind(Tup& t, F&& f) {
		return bind_impl(
			t,
			std::forward<F>(f),
			std::make_index_sequence<std::tuple_size_v<std::decay_t<Tup>>>{});
	}

	/////////////////////////////////////

	template <typename Tup1, typename Tup2, typename F, ysize... I1, ysize... I2>
	constexpr decltype(auto) bind_impl(Tup1& tup1, Tup2& tup2, F&& f, std::index_sequence<I1...>, std::index_sequence<I2...>) {
		return std::invoke(std::forward<F>(f), std::get<I1>(tup1)..., std::get<I2>(tup2)...);
	}

	template <typename Tup1, typename Tup2, typename F>
	constexpr decltype(auto) bind(Tup1& t1, Tup2& t2, F&& f) {
		return bind_impl(
			t1, t2,
			std::forward<F>(f),
			std::make_index_sequence<std::tuple_size_v<std::decay_t<Tup1>>>{},
			std::make_index_sequence<std::tuple_size_v<std::decay_t<Tup2>>>{});
	}
}

