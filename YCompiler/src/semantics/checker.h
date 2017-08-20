#pragma once

#include "../common.h"
#include "../syntax/position.h"
#include "../io/file_hnd.h"

struct AST_stmt;
struct type_cons;
struct symbol_table;

namespace checker {
	struct already_def_err {
		ystr Kind;
		ystr Name;
		yopt<interval> DefPos;
		interval RedefPos;

		already_def_err(ystr const& kind, ystr const& name,
			yopt<interval> defp, interval const& redefp)
			: Kind(kind), Name(name), DefPos(std::move(defp)), RedefPos(redefp) {
		}
	};

	struct type_unify_err {
		ystr Err;
		interval Pos;
		
		type_unify_err(ystr const& err, interval const& pos)
			: Err(err), Pos(pos) {
		}
	};

	struct undef_err {
		ystr Kind;
		ystr Name;
		interval Pos;

		undef_err(ystr const& kind, ystr const& name, interval const& pos)
			: Kind(kind), Name(name), Pos(pos) {
		}
	};

	using semantic_err = yvar<already_def_err, type_unify_err, undef_err>;

	using result_t = yopt<semantic_err>;

	extern file_hnd const* CurrentFile;
	extern symbol_table SymTab;

	extern type_cons* UNIT;
	extern type_cons* I32;
	extern type_cons* F32;

	void init();
	result_t check_program(file_hnd const& file, yvec<AST_stmt*>& prg);

	void handle_error(semantic_err& err);
}
