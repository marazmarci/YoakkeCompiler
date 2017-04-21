#pragma once

#include "../common.h"
#include "scope.h"
#include "../lexing/ytoken_t.h"
#include "type_symbol.h"

namespace yk {
	class symbol;
	class type_symbol;

	class symbol_table {
	public:
		static ysptr<type_symbol> UNIT_T;
		static ysptr<type_symbol> I32_T;
		static ysptr<type_symbol> F32_T;

	private:
		ysptr<scope> m_Global;
		ysptr<scope> m_Current;

	public:
		symbol_table();

	private:
		void decl_builtin_bin_op(ytoken_t op, ysptr<type_symbol> left, ysptr<type_symbol> right, ysptr<type_symbol> result);
		void decl_builtin_preury_op(ytoken_t op, ysptr<type_symbol> sub, ysptr<type_symbol> result);
		void decl_builtin_postury_op(ytoken_t op, ysptr<type_symbol> sub, ysptr<type_symbol> result);

	public:
		ysptr<type_symbol> create_bin_op_type(ysptr<type_symbol> left, ysptr<type_symbol> right, ysptr<type_symbol> result);
		ysptr<type_symbol> create_preury_op_type(ysptr<type_symbol> sub, ysptr<type_symbol> result);
		ysptr<type_symbol> create_postury_op_type(ysptr<type_symbol> sub, ysptr<type_symbol> result);

	public:
		void push(ysptr<scope>& sc);
		void pop();
		sym_set const* ref(ystr const& ident);
		void decl(ysptr<symbol> sym);
		ysptr<type_symbol> decl_type_once(ysptr<type_symbol> ref);

		template <typename T>
		static yvec<ysptr<T>> filter(sym_set const* set) {
			yvec<ysptr<T>> res;
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
		yvec<ysptr<T>> ref_filter(ystr const& id) {
			return filter<T>(ref(id));
		}

		template <typename T>
		static yvec<ysptr<T>> filter_typed_same(yvec<ysptr<T>> const& vec, ysptr<type_symbol> ty) {
			yvec<ysptr<T>> res;
			for (auto el : vec) {
				if (ty->same(el)) {
					res.push_back(el);
				}
			}
			return res;
		}

		template <typename T>
		static yvec<ysptr<T>> filter_typed_match(yvec<ysptr<T>> vec, ysptr<type_symbol> ty) {
			yvec<ysptr<T>> res;
			for (auto el : vec) {
				if (ty->match(el->Type)) {
					res.push_back(el);
				}
			}
			return res;
		}

		ysptr<type_symbol> decl_type_once(ystr const& id, ysptr<type_symbol> ref) {
			auto t_set = ref_filter<type_symbol>(id);
			t_set = filter_typed_same(t_set, ref);
			if (t_set.empty()) {
				// Create the symbol
				decl(ref);
				return ref;
			}
			else if (t_set.size() > 1) {
				// Sanity error
				throw std::exception("Sanity error: multiple definitions of the same type");
			}
			else {
				// Already exists one, grab it
				return t_set[0];
			}
		}
	};
}
