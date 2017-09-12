#include <iostream>
#include "checker.h"
#include "oper_desc.h"
#include "scope.h"
#include "symbol.h"
#include "symbol_table.h"
#include "type.h"
#include "unifier.h"
#include "../syntax/ast_stmt.h"
#include "../syntax/ast_expr.h"
#include "../syntax/ast_ty.h"
#include "../syntax/ast_pat.h"
#include "../io/fmt_code.h"
#include "../functions.h"

type_cons* checker::UNIT = type_cons::tuple();
type_cons* checker::I32	 = new type_cons("i32");
type_cons* checker::F32	 = new type_cons("f32");
type_cons* checker::BOOL = new type_cons("bool");

checker::checker(file_hnd const& f)
	: File(f) {
}

yopt<semantic_err> checker::check_program(yvec<AST_stmt*>& prg) {
	SymTab.decl("unit", UNIT);
	SymTab.decl("i32",	I32);
	SymTab.decl("f32",	F32);
	SymTab.decl("bool", BOOL);

	for (auto& stmt : prg) {
		if (auto err = phase1(stmt)) {
			return err;
		}
	}
	for (auto& stmt : prg) {
		if (auto err = phase2(stmt)) {
			return err;
		}
	}
	for (auto& stmt : prg) {
		if (auto err = phase3(stmt)) {
			return err;
		}
	}

	while (true) {
		auto res = unifier::process_class_constraint_list(Constraints);
		if (res.is_err()) {
			auto& err = res.get_err();
			return semantics_pos_err(
				"Semantic error: No matching overload found",
				err.Position
			);
		}
		auto& ok = res.get_ok();
		if (!ok) {
			break;
		}
	}
	if (Constraints.size()) {
		// TODO: Could report all, just report first for now
		return semantic_err(semantics_pos_err(
			"Semantic error: Could not choose corresponding overload",
			Constraints[0].Position
		));
	}

	return {};
}

yresult<type*, semantic_err> checker::check_ty(AST_ty* typ) {
	switch (typ->Ty) {
	case AST_ty_t::Ident: {
		auto ty = (AST_ident_ty*)typ;
		if (auto t = SymTab.ref_type(ty->Value)) {
			return *t;
		}
		return semantic_err(semantics_def_err(
			"Semantic error: Undefined %k %n %f!",
			"type", ty->Value, {}, to_sem_pos(ty->Pos)
		));
	}

	case AST_ty_t::List: {
		auto ty = (AST_list_ty*)typ;
		auto ty_sym = type_cons::tuple();
		for (auto& t : ty->Elements) {
			auto res = check_ty(t);
			if (res.is_err()) {
				return res.get_err();
			}
			ty_sym->add(res.get_ok());
		}
		return ty_sym;
	}

	case AST_ty_t::Bin: {
		auto ty = (AST_bin_ty*)typ;
		assert(ty->Oper.Type == token_t::Arrow);
		auto res_l = check_ty(ty->Left);
		if (res_l.is_err()) {
			return res_l.get_err();
		}
		auto res_r = check_ty(ty->Right);
		if (res_r.is_err()) {
			return res_r.get_err();
		}
		return type_cons::fn(res_l.get_ok(), res_r.get_ok());
	}

	case AST_ty_t::Pre:
	case AST_ty_t::Post: UNIMPLEMENTED;

	default: UNIMPLEMENTED;
	}

	UNREACHABLE;
	return UNIT;
}

type* checker::generate_let_pattern(AST_pat* pat, yvec<ytup<ystr, interval, type*>>& buff) {
	switch (pat->Ty) {
	case AST_pat_t::List: {
		auto pt = (AST_list_pat*)pat;
		type_cons* tlist = new type_cons(type_prefixes::Tuple);
		for (auto& elem : pt->Elements) {
			tlist->add(generate_let_pattern(elem, buff));
		}
		return tlist;
	}

	case AST_pat_t::Ident: {
		auto pt = (AST_ident_pat*)pat;
		type_var* tv = new type_var();
		buff.push_back({ pt->Value, pt->Pos, tv });
		return tv;
	}

	case AST_pat_t::Bin:
	case AST_pat_t::Pre:
	case AST_pat_t::Post: {
		UNIMPLEMENTED;
	}

	default: UNIMPLEMENTED;
	}

	UNREACHABLE;
	return nullptr;
}

ytup<yvec<ytup<ystr, interval, type*>>, type*> checker::generate_let_pattern(AST_pat* pat) {
	yvec<ytup<ystr, interval, type*>> buff;
	type* ty = generate_let_pattern(pat, buff);
	return { buff, ty };
}

///////////////////////////////////////////////////////////////////////////////

semantic_pos checker::to_sem_pos(interval const& pos) {
	return semantic_pos(File, pos);
}

type* checker::single_or(yvec<type*>& ts) {
	if (ts.size() == 1) {
		return *ts.begin();
	}
	return new type_cons(type_prefixes::Tuple, ts);
}

// TODO: Unifier formatting
void checker::print_def_msg(const char* fmt, const char* kind, ystr const& name,
	yopt<semantic_pos> defpos, semantic_pos const& redefpos) {
	auto& start = redefpos.Pos.Start;
	const char* ptr = fmt;
	while (char c = *ptr++) {
		if (c == '%') {
			c = *ptr++;
			switch (c) {
			case '%': {
				std::cout << '%';
				break;
			}

			case 'f': {
				std::cout << "in file: '"
					<< redefpos.File->path()
					<< "', line " << start.Row
					<< ", character " << start.Column;
				break;
			}

			case 'k': {
				std::cout << kind;
				break;
			}

			case 'n': {
				std::cout << '\'' << name << '\'';
				break;
			}

			default: UNIMPLEMENTED;
			}
		}
		else {
			std::cout << c;
		}
	}
	std::cout << std::endl;
	if (defpos) {
		auto& ddefpos = *defpos;
		auto& defstart = ddefpos.Pos.Start;
		if (semantic_pos::same_file(ddefpos, redefpos)) {
			fmt_code::print(*ddefpos.File, ddefpos.Pos, redefpos.Pos);
		}
		else {
			fmt_code::print(*redefpos.File, redefpos.Pos);
			std::cout
				<< "Note: Previous definition is in file: '"
				<< ddefpos.File->path()
				<< "' at line " << defstart.Row
				<< ", character " << defstart.Column << '.'
				<< std::endl;
			fmt_code::print(*ddefpos.File, ddefpos.Pos);
		}
	}
	else {
		fmt_code::print(*redefpos.File, redefpos.Pos);
	}
}

void checker::print_ty_msg(const char* fmt, ystr const& name1, ystr const& name2, 
	yopt<semantic_pos> const& pos1, semantic_pos const& pos2) {
	auto& start = pos2.Pos.Start;
	const char* ptr = fmt;
	while (char c = *ptr++) {
		if (c == '%') {
			c = *ptr++;
			switch (c) {
			case '%': {
				std::cout << '%';
				break;
			}

			case 'f': {
				std::cout << "in file: '"
					<< pos2.File->path()
					<< "', line " << start.Row
					<< ", character " << start.Column;
				break;
			}

			case 'a': {
				std::cout << '\'' << name1 << '\'';
				break;
			}

			case 'b': {
				std::cout << '\'' << name2 << '\'';
				break;
			}

			default: UNIMPLEMENTED;
			}
		}
		else {
			std::cout << c;
		}
	}
	std::cout << std::endl;
	if (pos1) {
		auto& ppos1 = *pos1;
		if (semantic_pos::same_file(ppos1, pos2)) {
			fmt_code::print(*ppos1.File, ppos1.Pos, pos2.Pos);
		}
		else {
			fmt_code::print(*pos2.File, pos2.Pos);
		}
	}
	else {
		fmt_code::print(*pos2.File, pos2.Pos);
	}
}

void checker::print_pointed_msg(const char* msg, semantic_pos const& pos) {
	auto& start = pos.Pos.Start;
	std::cout
		<< msg
		<< " in file: '"
		<< pos.File->path()
		<< "', line " << start.Row
		<< ", character " << start.Column
		<< '!' << std::endl;
	fmt_code::print(*pos.File, pos.Pos);
}

void checker::handle_error(semantic_err& err) {
	fnl::match(err)(
		[](semantics_def_err& err) {
			print_def_msg(err.Fmt, err.Kind, err.Name, err.DefPos, err.RedefPos);
		},
		[](semantics_ty_err& err) {
			print_ty_msg(err.Fmt, err.PrevName, err.CurrName, err.PrevPos, err.CurrPos);
		},
		[](semantics_pos_err& err) {
			print_pointed_msg(err.Msg.c_str(), err.Pos);
		}
	);
}
