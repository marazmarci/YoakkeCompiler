#include <iostream>
#include "checker.h"
#include "symbol_table.h"
#include "type.h"
#include "../functions.h"
#include "../syntax/ast_stmt.h"
#include "checker_phase1.h"
#include "checker_phase2.h"
#include "checker_phase3.h"
#include "../io/fmt_code.h"

namespace checker {
	file_hnd const* CurrentFile;
	symbol_table SymTab;

	type_cons* UNIT;
	type_cons* I32;
	type_cons* F32;

	void init() {
		SymTab.decl(UNIT = new type_cons(type_prefixes::Tuple));
		SymTab.decl(I32 = new type_cons("i32"));
		SymTab.decl(F32 = new type_cons("f32"));
	}

	result_t check_program(file_hnd const& file, yvec<AST_stmt*>& prg) {
		CurrentFile = &file;

		init();

		checker_phase1::init(SymTab);
		checker_phase2::init(SymTab);
		checker_phase3::init(SymTab);

		for (auto& st : prg) {
			if (auto err = checker_phase1::check(st)) {
				return *err;
			}
		}
		for (auto& st : prg) {
			if (auto err = checker_phase2::check(st)) {
				return *err;
			}
		}
		for (auto& st : prg) {
			if (auto err = checker_phase3::check(st)) {
				return *err;
			}
		}

		return {};
	}

	void handle_error(semantic_err& err) {
		fnl::match(err)(
			[](already_def_err& err) {
				auto& redef_pos = err.RedefPos;
				std::cout 
					<< "Semantic error: "
					<< err.Kind << " '" << err.Name 
					<< "' in file '"
					<< CurrentFile->path() << "' at line "
					<< redef_pos.Start.Row << ", character "
					<< redef_pos.Start.Column << " already defined!" << std::endl;
				if (err.DefPos) {
					// TODO: can be in other files, 
					// for now we just pretend it's in the same file
					fmt_code::print(*CurrentFile, *err.DefPos, redef_pos);
				}
				else {
					fmt_code::print(*CurrentFile, redef_pos);
				}
			}
		);
	}
}

