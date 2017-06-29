/**
 * val_symbol.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  Here are all the symbols representing values in the language.
 *  Variables, functions and constants are all values.
 */

#pragma once

#include "symbol.h"

struct ty_symbol;

/**
 * This enum contains all the types a value symbol can have.
 * This actually describes the assignment and usage. Other semantic
 * informations are contained inside the type symbol.
 */
enum class val_symbol_t {
	Const,
	Var,
};

/**
 * This is the base-class for all of the value symbols.
 * Every value symbol has an associated type symbol.
 */
struct val_symbol : public symbol {
public:
	val_symbol_t Category;	// The category of the value symbol
	ty_symbol* Type;		// The type symbol describing the value

protected:
	/**
	 * Creates a value symbol with a given name, category and type.
	 * @param name The name of the symbol.
	 * @param ty The category of the value.
	 * @param symt The type symbol of the value.
	 */
	val_symbol(ystr const& name, val_symbol_t ty, ty_symbol* symt);

public:
	virtual ~val_symbol();
};

/**
 * A variable value symbol can change during runtime, always needs to be
 * loaded.
 */
struct var_val_symbol : public val_symbol {
	/**
	 * Creates a variable value symbol with a given name and type.
	 * @param name The name of the symbol.
	 * @param symt The type symbol describing the type of the value.
	 */
	var_val_symbol(ystr const& name, ty_symbol* symt);

	virtual ~var_val_symbol();
};

/**
 * A constant value symbol is an all-time constant during compilation and
 * runtime. These are all the named functions and constants.
 */
struct const_val_symbol : public val_symbol {
	/**
	 * Creates a constant value symbol with a given name and type.
	 * @param name The name of the symbol.
	 * @param symt The type symbol describing the type of the value.
	 */
	const_val_symbol(ystr const& name, ty_symbol* symt);

	virtual ~const_val_symbol();
};
