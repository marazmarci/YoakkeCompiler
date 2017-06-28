#include <cassert>
#include "unifier.h"

namespace unifier {
	// TODO: Maybe return some success state?
	void unify(ty_symbol* ty1, ty_symbol* ty2) {
		auto c1 = ty1->Type;
		auto c2 = ty2->Type;

		if (c1 == ty_symbol_t::Construct && c2 == ty_symbol_t::Construct) {
			auto ct1 = static_cast<construct_ty_symbol*>(ty1);
			auto ct2 = static_cast<construct_ty_symbol*>(ty2);

			if (ct1->Name != ct2->Name) {
				// TODO: Error
				return;
			}
			if (ct1->Subtypes.size() != ct2->Subtypes.size()) {
				// TODO: Error
				return;
			}
			for (ysize i = 0; i < ct1->Subtypes.size(); i++) {
				unify(ct1->Subtypes[i], ct2->Subtypes[i]);
			}
		}
		else {
			assert(false && "Uncovered unifier case!");
		}
	}
}
