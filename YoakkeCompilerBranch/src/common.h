#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <vector>
#include <utility>
#include <tuple>

namespace yk {
	typedef std::uint8_t ybyte;
	typedef std::size_t ysize;
	typedef std::string ystr;

	template <typename K, typename V>
	using ymap = std::map<K, V>;

	template <typename T>
	using yset = std::set<T>;

	template <typename T>
	using yvec = std::vector<T>;

	template <typename T>
	using ystack = std::stack<T>;

	template <typename T, typename U>
	using ypair = std::pair<T, U>;

	template <typename T, typename U, typename V>
	using ytriplet = std::tuple<T, U, V>;
}