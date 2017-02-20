#pragma once

namespace yk {
	template <typename T, typename U>
	T base_of_impl(U T::*);
}

#define base_of(x) decltype(::yk::base_of_impl(x))
