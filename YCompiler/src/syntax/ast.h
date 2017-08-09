#pragma once

#include "position.h"
#include "token.h"

struct AST_node {
public:
	interval Pos;

protected:
	AST_node(interval const& pos);

public:
	virtual ~AST_node();
};
