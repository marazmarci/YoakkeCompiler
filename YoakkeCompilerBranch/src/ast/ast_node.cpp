#include "ast_node.h"
#include "expr.h"
#include "../utility/double_dispatcher.h"

namespace yk {
	ast_node::ast_node(position const& pos)
		: Position(pos) {
	}

	ast_node::~ast_node() { }

	struct tag_disp : public double_dispatcher
		<yset<ast_tag>, ast_node, 
		ident_expr, bin_expr, preury_expr, postury_expr, enclose_expr> {
	private:
		using tagset = yset<ast_tag>;

	public:
		tagset dispatch(ident_expr* e) override		{ return tagset{}; }
		tagset dispatch(bin_expr* e) override		{ return tagset{}; }
		tagset dispatch(preury_expr* e) override	{ return tagset{}; }
		tagset dispatch(postury_expr* e) override	{ return tagset{}; }
		tagset dispatch(enclose_expr* e) override	{ return tagset{}; }
	};
	tag_disp tag_dispatcher;

	yset<ast_tag> ast_node::get_tags(ast_node* n) {
		tag_dispatcher.dispatch_gen(n);
	}
}