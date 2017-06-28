/**
 * scope.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  This is the symbol table for semantic analysis.
 */

#pragma once

#include <cassert>
#include "scope.h"

template <typename T>
struct symbol_table {
	scope<T>* GlobalScope;
	scope<T>* CurrentScope;

	symbol_table() {
		GlobalScope = new scope<T>();
		CurrentScope = GlobalScope;
	}

	void pop() {
		assert(GlobalScope != CurrentScope && "Cannot pop global scope!");
		assert(CurrentScope->Parent && "Scope did not have a parent!");

		CurrentScope = CurrentScope->Parent;
	}

	void push(scope<T>* sc) {
		assert(sc->Parent == nullptr && "Cannot set multiple parents for scope!");
		
		sc->Parent = CurrentScope;
		CurrentScope = sc;
	}
};
