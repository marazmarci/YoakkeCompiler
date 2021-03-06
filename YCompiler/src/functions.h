#pragma once

#include <type_traits>
#include <variant>

namespace fnl {
	template <typename InputIt, typename T, typename BinaryOperation>
	T fold(InputIt first, InputIt last, T init, BinaryOperation op) {
		for (; first != last; ++first) {
			init = op(init, *first);
		}
		return init;
	}

	namespace internal__ {
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
	}

	template <typename... TVariants>
	constexpr auto match(TVariants&&... vs) {
		return [&vs...](auto&&... fs) -> decltype(auto) {
			auto visitor = internal__::overload(std::forward<decltype(fs)>(fs)...);
			return std::visit(visitor, std::forward<TVariants>(vs)...);
		};
	}

	namespace internal__ {
		template <typename Tup, typename F, ysize... I>
		constexpr decltype(auto) bind_impl(Tup& tup, F&& f, std::index_sequence<I...>) {
			return std::invoke(std::forward<F>(f), std::get<I>(tup)...);
		}
	}

	template <typename Tup, typename F>
	constexpr decltype(auto) bind(Tup& t, F&& f) {
		return internal__::bind_impl(
			t,
			std::forward<F>(f),
			std::make_index_sequence<std::tuple_size_v<std::decay_t<Tup>>>{}
		);
	}
}
