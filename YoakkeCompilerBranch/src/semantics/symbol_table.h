#pragma once

#include "scope.h"
#include "type_symbol.h"

namespace yk {
	class builtin_type_symbol;
	class type_symbol;

	class symbol_table {
	public:
		static builtin_type_symbol* UNIT;
		static builtin_type_symbol* INT32;
		static builtin_type_symbol* FLOAT32;

	private:
		scope* m_GlobalScope;
		scope* m_CurrentScope;

	public:
		symbol_table();

	private:
		void add_builtin_binop(ystr const& op, type_symbol* l, type_symbol* r, type_symbol* rett);
		void add_builtin_preop(ystr const& op, type_symbol* l, type_symbol* rett);
		void add_builtin_postop(ystr const& op, type_symbol* l, type_symbol* rett);

	public:
		bool is_global();
		void push(scope* sc);
		void pop();
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
		static yvec<T*> filter_typed_same(yvec<T*>& set, type_symbol* type) {
			yvec<T*> result;
			for (auto el : set) {
				if (el->Type->same(type)) {
					result.push_back(el);
				}
			}
			return result;
		}

		template <typename T>
		static yvec<T*> filter_typed_match(yvec<T*>& set, type_symbol* type) {
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