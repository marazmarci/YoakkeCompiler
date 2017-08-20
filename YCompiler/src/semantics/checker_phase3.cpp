#include "checker.h"
#include "checker_phase3.h"

namespace checker_phase3 {
	symbol_table& SymTab = checker::SymTab;

	void init(symbol_table& symt) {
		SymTab = symt;
	}
}