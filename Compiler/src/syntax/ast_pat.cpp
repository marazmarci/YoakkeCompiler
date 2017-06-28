#include "ast_pat.h"

// AST_pat

AST_pat::AST_pat(interval const& pos, AST_pat_t ty)
	: AST_node(pos), Type(ty) {
}

AST_pat::~AST_pat() { }

// AST_ident_pat

AST_ident_pat::AST_ident_pat(token const& tok)
	: AST_pat(tok.Pos, AST_pat_t::Ident), ID(tok.Value) {
}

AST_ident_pat::~AST_ident_pat() { }

// AST_list_ty

AST_list_pat::AST_list_pat(yvec<AST_pat*> const& elems)
	: AST_pat(interval((*elems.begin())->Pos,
	(*elems.rbegin())->Pos),
		AST_pat_t::List),
	Elements(elems) {
}

AST_list_pat::~AST_list_pat() {
	for (auto el : Elements) {
		delete el;
	}
}
