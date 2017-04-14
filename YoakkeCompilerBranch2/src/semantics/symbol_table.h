#pragma once

#include "../common.h"
#include "scope.h"

namespace yk {
	class symbol;
	class type_symbol;

	class symbol_table {
	public:
		static yshared_ptr<type_symbol> UNIT_T;
		static yshared_ptr<type_symbol> I32_T;
		static yshared_ptr<type_symbol> F32_T;

	private:
		yshared_ptr<scope> m_Global;
		yshared_ptr<scope> m_Current;

	public:
		symbol_table();

	public:
		void push(yshared_ptr<scope>& sc);
		void pop();
		sym_set const* ref(ystr const& ident);
		sym_set const* ref_global(ystr const& ident);
		void decl(yshared_ptr<symbol> sym);
		void decl_global(yshared_ptr<symbol> sym);

		template <typename T>
		static yvec<yshared_ptr<T>> filter(sym_set const* set) {
			yvec<yshared_ptr<T>> res;
			if (set) {
				for (auto const& sym : *set) {
					if (auto ss = std::dynamic_pointer_cast<T>(sym)) {
						res.push_back(ss);
					}
				}
			}
			return res;
		}

		template <typename T>
		yvec<yshared_ptr<T>> ref_filter(ystr const& id) {
			return filter<T>(ref(id));
		}

		template <typename T>
		yvec<yshared_ptr<T>> ref_global_filter(ystr const& id) {
			return filter<T>(ref_global(id));
		}
	};
}
