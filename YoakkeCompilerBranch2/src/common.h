#pragma once

#include <cstddef>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <variant>
#include <vector>
#include "utility\opt_map.h"

#define CAT_(x, y) x##y
#define CAT(x, y) CAT_(x, y)

namespace yk {
	using ysize = std::size_t;

	template <typename T>
	using yopt = std::optional<T>;

	using ystr = std::string;

	template<
		class Key,
		class T,
		class Compare = std::less<Key>,
		class Allocator = std::allocator<std::pair<const Key, T>>
	>
	using yopt_map = opt_map<Key, T, Compare, Allocator>;

	template<
		class Key,
		class T,
		class Compare = std::less<Key>,
		class Allocator = std::allocator<std::pair<const Key, T>>
	>
	using ymap = std::map<Key, T, Compare, Allocator>;

	template <typename T1, typename T2>
	using ypair = std::pair<T1, T2>;

	template <typename T, class Alloc = std::allocator<T>>
	using yvec = std::vector<T, Alloc>;

	template <typename T>
	using ysptr = std::shared_ptr<T>;

	template <typename T>
	using yset = std::set<T>;

	template <typename T1, typename T2, typename T3>
	using ytriplet = std::tuple<T1, T2, T3>;

	template<
		class Key,
		class T,
		class Hash = std::hash<Key>,
		class KeyEqual = std::equal_to<Key>,
		class Allocator = std::allocator< std::pair<const Key, T> >
	>
	using yhash_map = std::unordered_map<Key, T, Hash, KeyEqual, Allocator>;

	template< class T, class U >
	ysptr<T> dyn_cast(ysptr<U> const& r) {
		if (auto p = dynamic_cast<typename ysptr<T>::element_type*>(r.get())) {
			return ysptr<T>(r, p);
		}
		else {
			return ysptr<T>();
		}
	}

	template< class T, class U >
	ysptr<T> reinterpr_cast(ysptr<U> const& r) {
		if (auto p = reinterpret_cast<typename ysptr<T>::element_type*>(r.get())) {
			return ysptr<T>(r, p);
		}
		else {
			return ysptr<T>();
		}
	}

	template <typename... Ts>
	using yvar = std::variant<Ts...>;

	template <typename... Ts>
	using ytup = std::tuple<Ts...>;
}
