#pragma once

#include <cstddef>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include "utility\opt_map.h"

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
}
