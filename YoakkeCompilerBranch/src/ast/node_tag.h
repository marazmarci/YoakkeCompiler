#pragma once

#include "ast_node.h"
#include "../utility/optional.h"

namespace yk {
	class expr;

	class node_tag : public ast_node {
	public:
		ystr Name;

	protected:
		node_tag(point const& pos, token const& id);

	public:
		virtual ~node_tag();
	};

	class foreign_node_tag : public node_tag {
	public:
		yopt<token> Name;

	public:
		foreign_node_tag(token const& hashm, token& ident, yopt<token> param);
		virtual ~foreign_node_tag();
	};
}
