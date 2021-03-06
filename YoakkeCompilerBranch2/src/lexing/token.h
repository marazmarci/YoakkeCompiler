#pragma once

#include "position.h"
#include "ytoken_t.h"
#include "../common.h"

namespace yk {
	class token {
	public:
		ytoken_t	Type;
		ystr		Value;
		interval	Position;

	public:
		token();
		token(ytoken_t tt, ystr const& val, point const& pos);

	public:
		ystr format() const;
	};
}
