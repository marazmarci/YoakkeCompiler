#include "ast_pat.h"

AST_pat::AST_pat(interval const& pos, AST_pat_t ty)
	: AST_node(pos), Ty(ty) {
}

AST_pat::~AST_pat() { }

/*****************************************************************************/

AST_ident_pat::AST_ident_pat(token const& val)
	: AST_pat(val.Pos, AST_pat_t::Ident), Value(val.Value) {
}

AST_ident_pat::~AST_ident_pat() { }

/*****************************************************************************/

AST_pre_pat::AST_pre_pat(token const& op, AST_pat* sub)
	: AST_pat(interval(op.Pos, sub->Pos), AST_pat_t::Pre),
	Oper(op), Subexpr(sub) {
}

AST_pre_pat::~AST_pre_pat() {
	delete Subexpr;
}

/*****************************************************************************/

AST_bin_pat::AST_bin_pat(AST_pat* left, token const& op, AST_pat* right)
	: AST_pat(interval(left->Pos, right->Pos), AST_pat_t::Bin),
	Left(left), Oper(op), Right(right) {
}

AST_bin_pat::~AST_bin_pat() {
	delete Left;
	delete Right;
}

/*****************************************************************************/

AST_post_pat::AST_post_pat(AST_pat* sub, token const& op)
	: AST_pat(interval(sub->Pos, op.Pos), AST_pat_t::Post),
	Subexpr(sub), Oper(op) {
}

AST_post_pat::~AST_post_pat() {
	delete Subexpr;
}

/*****************************************************************************/

AST_list_pat::AST_list_pat(yvec<AST_pat*> const& elems)
	: AST_pat(interval((*elems.begin())->Pos, (*elems.rbegin())->Pos), AST_pat_t::List),
	Elements(elems) {
}

AST_list_pat::AST_list_pat(token const& beg, token const& end)
	: AST_pat(interval(beg.Pos, end.Pos), AST_pat_t::List),
	Elements(yvec<AST_pat*>{}) {
}

AST_list_pat::~AST_list_pat() {
	for (auto& el : Elements) {
		delete el;
	}
}
