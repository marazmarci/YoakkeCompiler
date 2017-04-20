#pragma once

#include <boost/vmd/vmd.hpp>
#include <boost\preprocessor\variadic\size.hpp>

#define EXPAND(x) x
#define CONCAT_(A, B) A ## B
#define CONCAT(A, B) CONCAT_(A, B)

#define Match(x) { auto to_be_matched__ = x;
#define EndMatch }

#define __DISP__(x)				auto& x = to_be_matched__2->x;

#define Dispatch0()
#define Dispatch1(x)								__DISP__(x);
#define Dispatch2(x, y)			Dispatch1(x)		__DISP__(y)
#define Dispatch3(x, y, z)		Dispatch2(x, y)		__DISP__(z)
#define Dispatch4(x, y, z, w)	Dispatch3(x, y, z)	__DISP__(w)

#define Dispatch_HANDLE0(...)	Dispatch0()
#define Dispatch_HANDLE1(...)	\
CONCAT(Dispatch, BOOST_PP_VARIADIC_SIZE(__VA_ARGS__))(__VA_ARGS__)

#define Dispatch(...) \
CONCAT(Dispatch_HANDLE, BOOST_VMD_SIZE(__VA_ARGS__))(__VA_ARGS__)

#define Case(x, ...) if (auto to_be_matched__2 = dynamic_cast<x*>(to_be_matched__)) { Dispatch(__VA_ARGS__)
#define EndCase }

#define CaseOther() {
