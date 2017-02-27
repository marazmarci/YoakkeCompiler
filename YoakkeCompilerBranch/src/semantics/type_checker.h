#pragma once

#include "../ast/type_desc.h"
#include "../utility/double_dispatcher.h"

namespace yk {
	class semantic_checker;
	class symbol_table;
	class type_symbol;

	class type_checker : public double_dispatcher
		<type_symbol*, type_desc,
		ident_type_desc, bin_type_desc, list_type_desc> {
	private:
		semantic_checker& m_Checker;
		symbol_table& m_Table;

	public:
		type_checker(semantic_checker& ch, symbol_table& tab);

	public:
		type_symbol* dispatch(ident_type_desc* td) override;
		type_symbol* dispatch(bin_type_desc* td) override;
		type_symbol* dispatch(list_type_desc* td) override;
	};
}