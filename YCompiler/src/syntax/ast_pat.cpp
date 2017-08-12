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
