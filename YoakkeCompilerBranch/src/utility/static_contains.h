#pragma once

#include <utility>

namespace yk {
	template <typename Element, typename... List>
	struct static_contains;

	template <typename Element>
	struct static_contains<Element> {
		static const bool value = false;
	};

	template <typename Element, typename Head>
	struct static_contains<Element, Head> {
		static const bool value = std::is_same<Element, Head>::value;
	};

	template <typename Element, typename Head, typename... Tail>
	struct static_contains<Element, Head, Tail...> {
		static const bool value = std::is_same<Element, Head>::value ?
			true : static_contains<Element, Tail...>::value;
	};
}