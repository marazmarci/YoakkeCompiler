#pragma once

#include "../ast/type_desc.h"
#include "../utility/double_dispatcher.h"

namespace yk {
	class semantic_checker;
	class symbol_table;

	class type_checker : public double_dispatcher
		<void, type_desc,
		ident_type_desc, bin_type_desc, enclose_type_desc> {
	private:
		semantic_checker& m_Checker;
		symbol_table& m_Table;

	public:
		type_checker(semantic_checker& ch, symbol_table& tab);

	public:
		void dispatch(ident_type_desc* stmt) override;
		void dispatch(bin_type_desc* stmt) override;
		void dispatch(enclose_type_desc* stmt) override;
	};
}