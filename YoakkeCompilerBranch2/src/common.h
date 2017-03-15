#pragma once

#include <optional>
#include <string>

namespace yk {
	template <typename T>
	using yopt = std::optional<T>;

	template <typename T>
	using ystr = std::string;
}
