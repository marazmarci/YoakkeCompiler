/**
 * common.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  These are the common type defines to be easily replacable if needed.
 */

#pragma once

#include <cstddef>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>

using ysize = std::size_t;
using ystr	= std::string;

template <typename T>
using yvec = std::vector<T>;

template <typename T>
using yref = std::reference_wrapper<T>;

template <typename T, typename U = T>
using ypair = std::pair<T, U>;

template <typename T>
using yset = std::set<T>;

template <typename K, typename T>
using ymap = std::map<K, T>;
