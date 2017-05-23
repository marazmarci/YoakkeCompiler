#include "symbol_table.h"
#include "scope.h"
#include "type.h"
#include "symbol.h"

namespace yk {
	type symbol_table::UNIT_T	= type::create_cons("unit");
	type symbol_table::I32_T	= type::create_cons("i32");
	type symbol_table::F32_T	= type::create_cons("f32");

	symbol_table::symbol_table() {
		m_GlobalScope	= std::make_shared<scope>();
		m_CurrentScope	= m_GlobalScope;
		m_GlobalTScope	= std::make_shared<ty_scope>();
		m_CurrentTScope = m_GlobalTScope;

		decl("unit",	UNIT_T);
		decl("i32",		I32_T);
		decl("f32",		F32_T);
	}

	void symbol_table::decl(symbol const& v) {
		m_CurrentScope->decl(v);
	}

	void symbol_table::decl(ystr const& id, type& t) {
		m_CurrentTScope->decl(id, t);
	}

	yopt<symbol> symbol_table::ref(ystr const& id) {
		return m_CurrentScope->ref(id);
	}

	yopt<type> symbol_table::ref_t(ystr const& id) {
		return m_CurrentTScope->ref(id);
	}

	scope* symbol_table::enclosing_return() {
		return m_CurrentScope->enclosing_return();
	}

	ysptr<scope> symbol_table::push(bool ret_dest) {
		auto sc = std::make_shared<scope>();
		sc->ReturnDest = ret_dest;
		sc->Parent = m_CurrentScope;
		m_CurrentScope = sc;
		auto tsc = std::make_shared<ty_scope>();
		tsc->Parent = m_CurrentTScope;
		m_CurrentTScope = tsc;
		return sc;
	}

	void symbol_table::pop() {
		if (!m_CurrentScope->Parent) {
			throw std::exception("Tried to pop global scope!");
		}
		if (!m_CurrentTScope->Parent) {
			throw std::exception("Tried to pop global type scope!");
		}
		m_CurrentScope = m_CurrentScope->Parent;
		m_CurrentTScope = m_CurrentTScope->Parent;
	}
}
