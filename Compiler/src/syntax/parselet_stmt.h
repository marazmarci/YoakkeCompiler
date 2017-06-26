#pragma once

struct AST_stmt;
struct parser;

namespace parselet_stmt {
	AST_stmt* get_stmt(parser& p);
}
