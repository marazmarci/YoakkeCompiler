#include "ast_ty.h"

// AST_ty

AST_ty::AST_ty(interval const& pos, AST_ty_t ty)
	: AST_node(pos), Type(ty) {
}

AST_ty::~AST_ty() { }

// AST_ident_ty

AST_ident_ty::AST_ident_ty(token const& tok)
	: AST_ty(tok.Pos, AST_ty_t::Ident), ID(tok.Value) {
}

AST_ident_ty::~AST_ident_ty() { }

// AST_bin_ty

AST_bin_ty::AST_bin_ty(token const& op, AST_ty* left, AST_ty* right)
	: AST_ty(interval(left->Pos, right->Pos), AST_ty_t::BinOp),
	OP(op), LHS(left), RHS(right) {
}

AST_bin_ty::~AST_bin_ty() {
	delete LHS;
	delete RHS;
}

// AST_list_ty

AST_list_ty::AST_list_ty(yvec<AST_ty*> const& elems)
	: AST_ty(interval((*elems.begin())->Pos,
	(*elems.rbegin())->Pos),
		AST_ty_t::List),
	Elements(elems) {
}

AST_list_ty::AST_list_ty(token const& beg, token const& end)
	: AST_ty(interval(beg.Pos, end.Pos), AST_ty_t::List) {
}

AST_list_ty::~AST_list_ty() {
	for (auto el : Elements) {
		delete el;
	}
}
