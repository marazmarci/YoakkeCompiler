/**
 * scope.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  Here is the symbol scope representation for semantic analysis.
 */

#pragma once

#include "../common.h"

template <typename T>
struct scope {
public:
	void decl(T* sym) {
		auto it = m_Symbols.find(sym->Name);
		if (it == m_Symbols.end()) {
			m_Symbols.insert({ sym->Name, yvec<T*>{ sym } });
		}
		else {
			it->second.push_back(sym);
		}
	}

	yvec<T*>* ref(ystr const& id) {
		auto it = m_Symbols.find(id);
		if (it == m_Symbols.end()) {
			// TODO: can we go to parent?
			if (Parent) {
				return Parent->ref(id);
			}
			return nullptr;
		}
		return &it->second;
	}

public:
	ymap<ystr, yvec<T*>>	m_Symbols;
	scope<T>*				Parent;
};
