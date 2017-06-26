#include "parselet_ty.h"

namespace parselet_ty {
	AST_ty* get_ty(parser& p, ysize prec) {
		return p.parse_ty(prec);
	}
}

