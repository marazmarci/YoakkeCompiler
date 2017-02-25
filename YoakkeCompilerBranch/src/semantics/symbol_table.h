#pragma once

#include "scope.h"

namespace yk {
	class symbol_table {
	private:
		scope* m_GlobalScope;
		scope* m_CurrentScope;

	public:
		symbol_table();

	public:
		bool is_global();
		void push(scope* sc);
		void pop(scope* sc);
		void decl(symbol* sym);
		sym_set* ref(ystr const& id);

	public:
		template <typename T>
		static yvec<T*> filter(sym_set* set) {
			yvec<T*> result;
			for (auto s : *set) {
				if (T* ss = dynamic_cast<T*>(s)) {
					result.push_back(ss);
				}
			}
			return result;
		}
	};
}