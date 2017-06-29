/**
 * sym_tab.h
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
struct sym_tab {
public:
	sym_tab() {
		m_GlobalScope = new scope<T>();
		m_CurrentScope = m_GlobalScope;
	}

	void pop() {
		assert(m_GlobalScope != m_CurrentScope && "Cannot pop global scope!");
		assert(m_CurrentScope->Parent && "Scope did not have a parent!");

		m_CurrentScope = CurrentScope->Parent;
	}

	void push(scope<T>* sc) {
		assert(sc->Parent == nullptr && "Cannot set multiple parents for scope!");
		
		sc->Parent = m_CurrentScope;
		m_CurrentScope = sc;
	}

	inline scope<T>* curr() {
		return m_CurrentScope;
	}

private:
	scope<T>* m_GlobalScope;
	scope<T>* m_CurrentScope;
};
