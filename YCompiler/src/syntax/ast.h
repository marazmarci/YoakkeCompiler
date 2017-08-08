#pragma once

#include "position.h"
#include "token.h"

struct AST_node {
	interval Pos;

	virtual ~AST_node() { }

protected:
	AST_node(interval const& pos)
		: Pos(pos) {
	}
};

struct label {
	interval	Pos;
	token		Name;

	label(interval const& pos, token const& name)
		: Pos(pos), Name(name) {
	}
};
