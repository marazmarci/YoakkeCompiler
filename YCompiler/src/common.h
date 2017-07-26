#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <tuple>
#include <map>
#include <variant>
#include <vector>

using ysize = std::size_t;
using ystr = std::string;

template <typename T>
using yvec = std::vector<T>;

template <typename... Ts>
using ytup = std::tuple<Ts...>;

using yunit = ytup<>;

template <typename T>
using yopt = std::optional<T>;

template <typename K, typename V>
using ymap = std::map<K, V>;

template <typename T, typename E>
using yvar = std::variant<T, E>;

template <typename T, typename E>
struct yresult {
public:
	using ok_type = T;
	using err_type = E;

public:
	yresult(T const& val)
		: m_Data(val) {
	}

	yresult(E const& err)
		: m_Data(err) {
	}

public:
	bool is_err() const {
		return std::holds_alternative<E>(m_Data);
	}

	bool is_ok() const {
		return std::holds_alternative<T>(m_Data);
	}

	E const& get_err() const {
		return std::get<1>(m_Data);
	}

	E& get_err() {
		return std::get<1>(m_Data);
	}

	T const& get_ok() const {
		return std::get<0>(m_Data);
	}

	T& get_ok() {
		return std::get<0>(m_Data);
	}

private:
	yvar<T, E> m_Data;
};
