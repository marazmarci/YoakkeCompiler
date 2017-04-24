#pragma once

#include <boost/vmd/is_empty.hpp>
#include <boost/preprocessor/variadic/size.hpp>

#define EXPAND(x) x

#define CAT_(x, y) x##y
#define CAT(x, y) CAT_(x, y)

#define VARIADIC_SIZE_1() 0
#define VARIADIC_SIZE_0(...) BOOST_PP_VARIADIC_SIZE(__VA_ARGS__)
#define VARIADIC_SIZE(...)											\
CAT(VARIADIC_SIZE_, BOOST_VMD_IS_EMPTY(__VA_ARGS__))(__VA_ARGS__)

#define Match(exp)						\
if (auto m__ = exp) if (auto f__ = true)

#define Case_0()
#define Case_1(x) for (auto& x = m2__->x; f__; f__ = false)
#define Case_2(x, y)	Case_1(x) Case_1(y)
#define Case_3(x, y, z) Case_2(x, y) Case_1(z)
#define Case_4(x, y, z, w) Case_3(x, y, z) Case_1(w)

#define Case(type, ...)						\
if (auto m2__ = dynamic_cast<type*>(m__))	\
EXPAND(CAT(Case_, VARIADIC_SIZE(__VA_ARGS__))(__VA_ARGS__))

#define Otherwise() if (f__)
