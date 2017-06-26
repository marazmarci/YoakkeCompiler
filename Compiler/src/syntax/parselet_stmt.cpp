#include "parselet_stmt.h"
#include "ast_stmt.h"
#include "parser.h"

namespace parselet_stmt {
	AST_stmt* get_stmt(parser& p) {
		return p.parse_stmt();
	}
}
