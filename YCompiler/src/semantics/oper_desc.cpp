#include "oper_desc.h"
#include "type.h"
#include "unifier.h"

namespace oper_desc {
	namespace {
		type_cons* UNARY_ARGS	= type_cons::generic_params(1);
		type_cons* BINARY_ARGS	= type_cons::generic_params(2);

		bool match1(type* def_arg_t) {
			return unifier::matches(def_arg_t, UNARY_ARGS);
		}

		bool match2(type* def_arg_t) {
			return unifier::matches(def_arg_t, BINARY_ARGS);
		}
	}

	bool good_def(token_t op, type* def_arg_t) {
		switch (op) {
		case token_t::Add:
		case token_t::Sub: {
			return match1(def_arg_t) || match2(def_arg_t);
		}

		case token_t::Eq:
		case token_t::Neq:
		case token_t::Greater:
		case token_t::Less:
		case token_t::GrEq:
		case token_t::LeEq:
		case token_t::Mul:
		case token_t::Div:
		case token_t::Mod: {
			return match2(def_arg_t);
		}

		default: UNIMPLEMENTED;
		}

		UNREACHABLE;
		return false;
	}
}
