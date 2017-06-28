/**
 * unifier.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  Here is the unifying logic for the type system of Yoakke.
 */

#pragma once

#include "ty_symbol.h"

/**
 * TODO: Reference unification rules
 */

namespace unifier {
	/**
	 * Unifies two types based on the unification rules.
	 * @param ty1 One type to unify.
	 * @param ty2 The other type to unify.
	 */
	void unify(ty_symbol* ty1, ty_symbol* ty2);
}
