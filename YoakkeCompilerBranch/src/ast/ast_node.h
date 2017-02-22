#pragma once

#include "../parse/position.h"
#include "ast_tag.h"

namespace yk {
	class ast_node {
	public:
		const position Position;

	public:
		ast_node(position const& pos);
		virtual ~ast_node();

	public:
		static yset<ast_tag> get_tags(ast_node* n);
	};
}