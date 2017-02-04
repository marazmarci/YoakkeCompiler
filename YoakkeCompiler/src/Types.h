#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace yk
{
	typedef int yint;
	typedef int yindex;
	typedef unsigned int yuint;
	typedef std::size_t ysize;
	typedef std::string ystr;

	template <typename T, typename U>
	using ymap = std::map<T, U>;

	template <typename T>
	using yvec = std::vector<T>;

	template <typename T>
	using yset = std::set<T>;
}