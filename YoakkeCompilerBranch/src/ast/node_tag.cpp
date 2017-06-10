#include "node_tag.h"

namespace yk {
	node_tag::node_tag(point const& pos, token const& id)
		: ast_node(pos), Name(id.value()) {
	}

	node_tag::~node_tag() { }

	foreign_node_tag::foreign_node_tag(token const& hashm, token& ident, yopt<token> param) 
		: node_tag(point::interval(point::get(hashm), 
			point::get(param.some() ? param.get() : ident)), ident), Name(param) {
	}

	foreign_node_tag::~foreign_node_tag() { }
}