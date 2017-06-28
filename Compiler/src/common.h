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
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

using ysize = std::size_t;
using ystr	= std::string;

template <typename T>
using yvec = std::vector<T>;

template <typename T, typename U>
using ypair = std::pair<T, U>;

template <typename K, typename T>
using ymap = std::map<K, T>;

template <typename T>
using yref = std::reference_wrapper<T>;

template <typename T>
using yopt = std::optional<T>;

template <typename K, typename V>
using yhash_map = std::unordered_map<K, V>;
