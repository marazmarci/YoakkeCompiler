#pragma once

#include "preproc.h"

#define static_block STATIC_BLOCK_IMPL1(UNIQUE_IDENT(_static_block_))
#define STATIC_BLOCK_IMPL1(prefix) STATIC_BLOCK_IMPL2(CONCAT(prefix, _fn), CONCAT(prefix, _var))
#define STATIC_BLOCK_IMPL2(func_name, var_name)		\
			static void func_name();				\
			static int var_name = (func_name(), 0);	\
			static void func_name()