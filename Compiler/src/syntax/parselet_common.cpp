#include "parselet_common.h"
#include "parser.h"

namespace parselet_common {
	yopt<token> get_label(parser& p) {
		return 
		enclose<yopt<token>>(
			get_token<token_t::LBracket>,
			get_token<token_t::Ident>,
			get_token<token_t::RBracket>,
			"identifier",
			"']'"
		)(p);
	}
}
