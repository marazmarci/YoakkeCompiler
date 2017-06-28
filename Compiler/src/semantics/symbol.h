/**
 * symbol.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  This file contains the base-class for all of the symbols in the semantic 
 *  analysis.
 */

#pragma once

#include "../common.h"

/**
 * A symbol is anything, that has a name. A function, a variable, a type and
 * so on. This is the base-class for all of the different symbols.
 */
struct symbol {
public:
	ystr Name;	// Name of the symbol

protected:
	/**
	 * Creates a symbol with a given name.
	 * @param name The name of the symbol.
	 */
	symbol(ystr const& name)
		: Name(name) {
	}

public:
	virtual ~symbol() { }
};
