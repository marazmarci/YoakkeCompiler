#include "sem_checker.h"
#include "ty_symbol.h"
#include "val_symbol.h"
#include "unifier.h"
#include "../syntax/ast_stmt.h"
#include "../syntax/ast_expr.h"
#include "../syntax/ast_ty.h"

ty_symbol* sem_checker::I32		= new construct_ty_symbol("i32");
ty_symbol* sem_checker::BOOL	= new construct_ty_symbol("bool");
ty_symbol* sem_checker::UNIT	= new construct_ty_symbol("tuple");

sem_checker::sem_checker() {
	m_Types.curr()->decl(I32);
	m_Types.curr()->decl(BOOL);
	// Noo need for unit, thet is constructed
}

void sem_checker::check(AST_stmt* st) {
	switch (st->Type) {
	case AST_stmt_t::Expr: {
		auto n = static_cast<AST_expr_stmt*>(st);
		check(n->Sub);
		return;
	}
	}

	assert(false && "Uncovered case for statement!");
}

ty_symbol* sem_checker::check(AST_expr* exp) {
	switch (exp->Type) {
	case AST_expr_t::BinOp: {
		// TODO
		return nullptr;
	}

	case AST_expr_t::Call: {
		auto n = static_cast<AST_call_expr*>(exp);
		// TODO: add hints based on args
		auto fn_ty = check(n->Func);
		if (fn_ty->Name != "function") {
			// TODO: error, cannot call non-function type!
			return nullptr;
		}
		auto f_ty = static_cast<construct_ty_symbol*>(fn_ty);
		if (f_ty->Subtypes.size() != n->Args.size() + 1) {
			// TODO: error, wrong no. args!
		}
		for (ysize i = 0; i < n->Args.size(); i++) {
			auto arg_t = check(n->Args[i]);
			// TODO: check for errors
			unifier::unify(arg_t, f_ty->Subtypes[i]);
		}
		return *f_ty->Subtypes.rbegin();
	}

	case AST_expr_t::Func: {
		// TODO: check for global statements that make no sense
		auto n = static_cast<AST_func_expr*>(exp);

		yvec<ty_symbol*> types;
		for (auto arg : n->Args) {
			types.push_back(check(arg.second));
		}
		auto ret_t = n->Return ? check(n->Return) : UNIT;
		types.push_back(ret_t);

		auto my_ty = new construct_ty_symbol("function", types);
		if (n->Label) {
			// We have a name, try to declare it in the scope
			ystr const& name = n->Label->Value;
			if (m_Values.curr()->ref(name)) {
				// TODO: error? If not overloadable
				// Maybe just let shadowing and warn?
			}
			m_Values.curr()->decl(new const_val_symbol(
				name,
				my_ty
			));
		}

		// Now we have declared the function, go into the body semantics
		push_scope();

		// Arguments
		yset<ystr> arg_names;
		for (ysize i = 0; i < n->Args.size(); i++) {
			if (n->Args[i].first) {
				ystr const& arg_n = n->Args[i].first->Value;
				if (arg_names.find(arg_n) != arg_names.end()) {
					// TODO: error, multiple args w same name
				}
				else {
					m_Values.curr()->decl(new var_val_symbol(
						arg_n,
						types[i]
					));
				}
			}
			else {
				// TODO: warn unnamed param
			}
		}

		// Check the body
		auto body_t = check(n->Body);

		// TODO: check body type and return type
		unifier::unify(body_t, ret_t);

		pop_scope();

		return my_ty;
	}

	case AST_expr_t::Ident: {
		auto n = static_cast<AST_ident_expr*>(exp);
		// TODO: helper types for filtering
		auto* res = m_Values.curr()->ref(n->ID);
		if (!res) {
			// TODO: error
			return nullptr;
		}
		// Get the last one for now
		auto* last_val = *res->rbegin();
		return last_val->Type;
	}

	case AST_expr_t::If: {
		// TODO
		return nullptr;
	}

	case AST_expr_t::IntLit: {
		// TODO
		return nullptr;
	}

	case AST_expr_t::List: {
		// TODO
		return nullptr;
	}

	case AST_expr_t::PostOp: {
		// TODO
		return nullptr;
	}

	case AST_expr_t::PreOp: {
		// TODO
		return nullptr;
	}
	}

	assert(false && "Uncovered case for expression!");
}

ty_symbol* sem_checker::check(AST_body_expr* body) {
	push_scope();

	for (auto st : body->Statements) {
		check(st);
	}
	auto rett = body->Return ? check(body->Return) : UNIT;

	pop_scope();

	// TODO: the return should go to the appropriate place
	// If it is named
	return rett;
}

ty_symbol* sem_checker::check(AST_ty* ty) {
	switch (ty->Type) {
	case AST_ty_t::BinOp: {
		// TODO
		return nullptr;
	}

	case AST_ty_t::Ident: {
		auto n = static_cast<AST_ident_ty*>(ty);

		auto ty = m_Types.curr()->ref(n->ID);
		if (!ty) {
			// TODO: error no such type
			return nullptr;
		}
		// Return the last one
		return *ty->rbegin();
	}

	case AST_ty_t::List: {
		// TODO
		return nullptr;
	}
	}
	assert(false && "Uncovered case for type!");
}

void sem_checker::push_scope() {
	m_Values.push();
	m_Types.push();
}

void sem_checker::pop_scope() {
	m_Values.pop();
	m_Types.pop();
}
