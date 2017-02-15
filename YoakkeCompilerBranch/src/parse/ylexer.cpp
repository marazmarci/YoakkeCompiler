#include "ylexer.h"

namespace yk {
	ylexer::ylexer() {
		// Skip whitespace
		add_skip(lr::mul(lr::set(" \t\n")));
		// Skip comment
		// Single-line comment
		auto sl_comm = lr::match("//");
		auto sl_comm_cont = lr::mul(lr::range(1, 127));
		sl_comm->set_next(sl_comm_cont);
		sl_comm_cont->set_next(lr::set("\n"));
		add_skip(sl_comm);
		// Multi-line comment 
		auto mul_comm = lr::match("/*");
		auto mul_comm_cont = lr::mul(lr::or({ mul_comm, lr::range(1, 127) }));
		mul_comm->set_next(mul_comm_cont);
		mul_comm_cont->set_next(lr::match("*/"));
		add_skip(mul_comm);

		// Keywords
		// Identifier
		auto ident = lr::or({ lr::range('a', 'z'), lr::range('A', 'Z'), lr::set("_") });
		ident->set_next(
			lr::mul(lr::or({ lr::range('a', 'z'), lr::range('A', 'Z'), lr::range('0', '9'),
				lr::set("_") })));
		add_rule(ident, "Identifier");
	}
}