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
