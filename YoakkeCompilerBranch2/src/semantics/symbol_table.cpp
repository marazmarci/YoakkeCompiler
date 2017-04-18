#include "symbol_table.h"
#include "symbol.h"
#include "type_symbol.h"
#include "typed_symbol.h"

namespace yk {
	ysptr<type_symbol> symbol_table::UNIT_T
		= std::make_shared<native_type_symbol>("unit");
	ysptr<type_symbol> symbol_table::I32_T
		= std::make_shared<native_type_symbol>("i32");
	ysptr<type_symbol> symbol_table::F32_T
		= std::make_shared<native_type_symbol>("f32");

	symbol_table::symbol_table() {
		m_Global = std::make_shared<scope>();
		m_Current = m_Global;

		// Add native types

		decl(UNIT_T);
		decl(I32_T);
		decl(F32_T);

		// Add native operators
		
		//									 (A  ,  B)  ->  C
		decl_builtin_bin_op(ytoken_t::Add, I32_T, I32_T, I32_T);
		decl_builtin_bin_op(ytoken_t::Sub, I32_T, I32_T, I32_T);
		decl_builtin_bin_op(ytoken_t::Mul, I32_T, I32_T, I32_T);
		decl_builtin_bin_op(ytoken_t::Div, I32_T, I32_T, I32_T);
		decl_builtin_bin_op(ytoken_t::Mod, I32_T, I32_T, I32_T);
	}

	void symbol_table::decl_builtin_bin_op(ytoken_t op, ysptr<type_symbol> left, ysptr<type_symbol> right, ysptr<type_symbol> result) {
		auto params = std::make_shared<tuple_type_symbol>
			(yvec<ysptr<type_symbol>>{ left, right });
		auto params2 = decl_type_once(params);
		auto ftype = std::make_shared<fn_type_symbol>(params2, result);
		auto ftype2 = decl_type_once(ftype);

		auto sym = builtin_typed_symbol::create_bin_op(op, ftype2);
		decl(sym);
	}

	void symbol_table::push(ysptr<scope>& sc) {
		sc->set_parent(m_Current);
		m_Current = sc;
	}

	void symbol_table::pop() {
		if (auto par = m_Current->get_parent()) {
			m_Current = par;
		}
		else {
			throw std::exception("Cannot pop global scope!");
		}
	}

	sym_set const* symbol_table::ref(ystr const& ident) {
		if (auto set = m_Current->ref(ident)) {
			return set.value();
		}
		else {
			return nullptr;
		}
	}

	void symbol_table::decl(ysptr<symbol> sym) {
		m_Current->decl(sym);
	}

	ysptr<type_symbol> symbol_table::decl_type_once(ysptr<type_symbol> ref) {
		auto t_set = ref_filter<type_symbol>(ref->Identifier);
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
}
