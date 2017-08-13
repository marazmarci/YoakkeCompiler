#include "ast_ty.h"

AST_ty::AST_ty(interval const& pos, AST_ty_t ty)
	: AST_node(pos), Ty(ty) {
}

AST_ty::~AST_ty() { }

/*****************************************************************************/

AST_ident_ty::AST_ident_ty(token const& val)
	: AST_ty(val.Pos, AST_ty_t::Ident), Value(val.Value) {
}

AST_ident_ty::~AST_ident_ty() { }

/*****************************************************************************/

AST_pre_ty::AST_pre_ty(token const& op, AST_ty* sub)
	: AST_ty(interval(op.Pos, sub->Pos), AST_ty_t::Pre),
	Oper(op), Subexpr(sub) {
}

AST_pre_ty::~AST_pre_ty() {
	delete Subexpr;
}

/*****************************************************************************/

AST_bin_ty::AST_bin_ty(AST_ty* left, token const& op, AST_ty* right)
	: AST_ty(interval(left->Pos, right->Pos), AST_ty_t::Bin),
	Left(left), Oper(op), Right(right) {
}

AST_bin_ty::~AST_bin_ty() {
	delete Left;
	delete Right;
}

/*****************************************************************************/

AST_post_ty::AST_post_ty(AST_ty* sub, token const& op)
	: AST_ty(interval(sub->Pos, op.Pos), AST_ty_t::Post),
	Subexpr(sub), Oper(op) {
}

AST_post_ty::~AST_post_ty() {
	delete Subexpr;
}

/*****************************************************************************/

AST_list_ty::AST_list_ty(yvec<AST_ty*> const& elems)
	: AST_ty(interval((*elems.begin())->Pos, (*elems.rbegin())->Pos), AST_ty_t::List),
	Elements(elems) {
}

AST_list_ty::AST_list_ty(token const& beg, token const& end)
	: AST_ty(interval(beg.Pos, end.Pos), AST_ty_t::List),
	Elements(yvec<AST_ty*>{}) {
}

AST_list_ty::~AST_list_ty() {
	for (auto& el : Elements) {
		delete el;
	}
}
