#include "let_meta.h"

namespace yk {
	void let_meta::add(ystr const& id, expr* val, type_symbol* ts) {
		Values.push_back(std::make_tuple(id, val, ts));
	}
}