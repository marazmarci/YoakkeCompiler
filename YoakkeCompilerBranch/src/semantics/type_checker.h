#pragma once

#include "../ast/type_desc.h"
#include "../utility/visitor.h"

namespace yk {
	class semantic_checker;
	class symbol_table;
	class type_symbol;

	class type_checker : public visitor<type_desc, type_symbol*> {
	public:
		META_Visitor(type_checker, check)

	private:
		semantic_checker& m_Checker;
		symbol_table& m_Table;

	public:
		type_checker(semantic_checker& ch, symbol_table& tab);

	public:
		type_symbol* check(type_desc& td);
		type_symbol* check(ident_type_desc& td);
		type_symbol* check(bin_type_desc& td);
		type_symbol* check(list_type_desc& td);
	};
}