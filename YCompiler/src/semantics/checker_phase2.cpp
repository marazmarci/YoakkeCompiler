#include "checker.h"
#include "checker_phase2.h"

namespace checker_phase2 {
	symbol_table& SymTab = checker::SymTab;

	void init(symbol_table& symt) {
		SymTab = symt;
	}
}