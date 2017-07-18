#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

using ysize = std::size_t;
using ystr = std::string;

template <typename T>
using yvec = std::vector<T>;

template <typename... Ts>
using ytup = std::tuple<Ts...>;

using unit = ytup<>;

template <typename T>
using yopt = std::optional<T>;
