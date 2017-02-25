#pragma once

namespace yk {
	class symbol_table;
	class pattern;
	class type_symbol;

	namespace let_pat {
		void define(symbol_table& table, pattern* left, type_symbol* right);
	}
}