#include "ast_ty.h"

AST_ty::AST_ty(interval const& pos, AST_ty_t ty)
	: AST_node(pos), Ty(ty) {
}

AST_ty::~AST_ty() { }

/*****************************************************************************/

AST_ident_ty::AST_ident_ty(token const& val)
	: AST_ty(val.Pos, AST_ty_t::Ident) {
}

AST_ident_ty::~AST_ident_ty() { }

/*****************************************************************************/
