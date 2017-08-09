#include "ast.h"

AST_node::AST_node(interval const& pos)
	: Pos(pos) {
}

AST_node::~AST_node() { }