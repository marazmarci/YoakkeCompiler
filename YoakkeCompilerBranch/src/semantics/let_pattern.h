#pragma once

namespace yk {
	class symbol_table;
	class pattern;
	class type_symbol;
	class let_meta;
	class expr;

	namespace let_pat {
		void define(symbol_table& table, pattern* left, type_symbol* right, expr* exp, let_meta& lm);
	}
}