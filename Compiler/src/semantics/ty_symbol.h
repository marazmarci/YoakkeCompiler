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
	Native,
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
