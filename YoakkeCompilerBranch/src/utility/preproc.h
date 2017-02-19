#pragma once

#define EXPAND(x) x

#define CONCAT(x, y) x##y
#define CONCAT_EXP(x, y) CONCAT(x, y)

#define UNIQUE_IDENT(prefix) CONCAT_EXP(prefix, __LINE__)