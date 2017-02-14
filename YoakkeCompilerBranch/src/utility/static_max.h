#pragma once

/*
Get the maximum value from variadic template arguments.
*/

namespace yk {
	template <typename T, T Head, T... Tail>
	struct static_max;

	template <typename T, T Head>
	struct static_max<T, Head> {
		static const T value = Head;
	};

	template <typename T, T Head1, T Head2, T... Tail>
	struct static_max<T, Head1, Head2, Tail...> {
		static const T value = Head1 >= Head2 ?
			static_max<T, Head1, Tail...>::value : static_max<T, Head2, Tail...>::value;
	};
}