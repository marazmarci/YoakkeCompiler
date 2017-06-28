/**
 * ty_symbol.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  Here are all the symbols representing types in the language.
 */

#pragma once

#include "symbol.h"

 /**
  * This enum contains all the types an type symbol can have.
  * Used for faster casting.
  */
enum class ty_symbol_t {
	Construct,
};

/**
 * This is the base-class for all of the type symbols.
 */
struct ty_symbol : public symbol {
public:
	ty_symbol_t Type;	// The category of the type symbol

protected:
	/**
	 * Creates a type symbol with a given name and category.
	 * @param name The name of the symbol.
	 * @param ty The category of the type.
	 */
	ty_symbol(ystr const& name, ty_symbol_t ty);

public:
	virtual ~ty_symbol();
};

/**
 * A construct type is a name and a list of types constructing it.
 * Example: integer (0 constructor types), vector<float> (1 constructor type),
 * tuple<...> (any number of constructor types).
 */
struct construct_ty_symbol : public ty_symbol {
	yvec<ty_symbol*> Subtypes;	// The constructor types

	/**
	 * Creates a construct type with a name and a list of subtypes.
	 * @param name The name of the construct type.
	 * @param tys The constructor subtypes (optional).
	 */
	construct_ty_symbol(ystr const& name, yvec<ty_symbol*> const& tys = yvec<ty_symbol*>{});

	virtual ~construct_ty_symbol();
};

