#pragma once

#include "scope.h"

namespace yk {
	class builtin_type_symbol;

	class symbol_table {
	public:
		static builtin_type_symbol* UNIT;
		static builtin_type_symbol* INT32;

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
			if (set) {
				for (auto s : *set) {
					if (T* ss = dynamic_cast<T*>(s)) {
						result.push_back(ss);
					}
				}
			}
			return result;
		}

		template <typename T>
		static yvec<T*> filter_typed(yvec<T*>& set, type_symbol* type) {
			yvec<T*> result;
			for (auto el : set) {
				if (el->Type->match(type)) {
					result.push_back(el);
				}
			}
			return result;
		}
	};
}