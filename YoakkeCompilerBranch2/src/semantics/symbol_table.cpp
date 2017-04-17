#include "symbol_table.h"
#include "symbol.h"
#include "type_symbol.h"

namespace yk {
	yshared_ptr<type_symbol> symbol_table::UNIT_T
		= std::make_shared<native_type_symbol>("unit");
	yshared_ptr<type_symbol> symbol_table::I32_T
		= std::make_shared<native_type_symbol>("i32");
	yshared_ptr<type_symbol> symbol_table::F32_T
		= std::make_shared<native_type_symbol>("f32");

	symbol_table::symbol_table() {
		m_Global = std::make_shared<scope>();
		m_Current = m_Global;

		// Add native types

		decl(UNIT_T);
		decl(I32_T);
		decl(F32_T);

		// Add native operators


	}

	void symbol_table::push(yshared_ptr<scope>& sc) {
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

	sym_set const* symbol_table::ref_global(ystr const& ident) {
		if (auto set = m_Global->ref(ident)) {
			return set.value();
		}
		else {
			return nullptr;
		}
	}

	void symbol_table::decl(yshared_ptr<symbol> sym) {
		m_Current->decl(sym);
	}

	void symbol_table::decl_global(yshared_ptr<symbol> sym) {
		m_Global->decl(sym);
	}
}
