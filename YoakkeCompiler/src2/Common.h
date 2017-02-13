#pragma once

#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <tuple>

namespace yk
{
	typedef std::size_t ysize;
	typedef std::string ystr;

	template <typename T>
	using yvec = std::vector<T>;

	template <typename T>
	using yset = std::set<T>;

	template <typename T, typename U>
	using ymap = std::map<T, U>;

	template <typename T, typename U>
	using ypair = std::pair<T, U>;

	template <typename T>
	using yhpair = std::pair<T, T>;

	template <typename T1, typename T2, typename T3>
	using ytriplet = std::tuple<T1, T2, T3>;

	template <typename T1, typename T2, typename T3, typename T4>
	using yquad = std::tuple<T1, T2, T3, T4>;
}