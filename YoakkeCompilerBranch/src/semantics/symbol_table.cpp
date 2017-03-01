#include "symbol_table.h"
#include "scope.h"
#include "type_symbol.h"
#include "typed_symbol.h"

namespace yk {
	builtin_type_symbol* symbol_table::UNIT = nullptr;
	builtin_type_symbol* symbol_table::INT32 = nullptr;
	builtin_type_symbol* symbol_table::FLOAT32 = nullptr;

	symbol_table::symbol_table() {
		m_GlobalScope = new scope();
		m_CurrentScope = m_GlobalScope;

		decl(UNIT = new builtin_type_symbol("unit"));
		decl(INT32 = new builtin_type_symbol("i32"));
		decl(FLOAT32 = new builtin_type_symbol("f32"));

		// Builtin operations
		// Integer
		add_builtin_binop("+", INT32, INT32, INT32);
		add_builtin_binop("-", INT32, INT32, INT32);
		add_builtin_binop("*", INT32, INT32, INT32);
		add_builtin_binop("/", INT32, INT32, INT32);
		add_builtin_binop("%", INT32, INT32, INT32);

		// Float
		add_builtin_binop("+", FLOAT32, FLOAT32, FLOAT32);
		add_builtin_binop("-", FLOAT32, FLOAT32, FLOAT32);
		add_builtin_binop("*", FLOAT32, FLOAT32, FLOAT32);
		add_builtin_binop("/", FLOAT32, FLOAT32, FLOAT32);
		add_builtin_binop("%", FLOAT32, FLOAT32, FLOAT32);
	}

	void symbol_table::add_builtin_binop(ystr const& op, type_symbol* l, type_symbol* r, type_symbol* rett) {
		decl(new builtin_function_symbol("bin_op" + op,
			new func_type_symbol(yvec<type_symbol*>{l, r}, rett)));
	}

	void symbol_table::add_builtin_preop(ystr const& op, type_symbol* l, type_symbol* rett) {
		decl(new builtin_function_symbol("pre_op" + op,
			new func_type_symbol(yvec<type_symbol*>{l}, rett)));
	}

	void symbol_table::add_builtin_postop(ystr const& op, type_symbol* l, type_symbol* rett) {
		decl(new builtin_function_symbol("post_op" + op,
			new func_type_symbol(yvec<type_symbol*>{l}, rett)));
	}

	bool symbol_table::is_global() {
		return m_GlobalScope == m_CurrentScope;
	}

	void symbol_table::push(scope* sc) {
		m_CurrentScope->Children.push_back(sc);
		sc->Parent = m_CurrentScope;
		m_CurrentScope = sc;
	}

	void symbol_table::pop() {
		if (m_CurrentScope == m_GlobalScope) {
			throw std::exception("Cannot pop global scope!");
		}
		else {
			m_CurrentScope = m_CurrentScope->Parent;
		}
	}

	void symbol_table::decl(symbol* sym) {
		m_CurrentScope->decl(sym);
	}

	sym_set* symbol_table::ref(ystr const& id) {
		return m_CurrentScope->ref(id);
	}
}
