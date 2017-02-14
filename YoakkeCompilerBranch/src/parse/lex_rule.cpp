#include "lex_rule.h"

namespace yk {
	lex_rule::~lex_rule() { }

	and_lex_rule::and_lex_rule(lex_rule* l, lex_rule* r)
		: m_Left(l), m_Right(r) {
	}

	and_lex_rule::~and_lex_rule() {
		delete m_Left;
		delete m_Right;
	}

	ysize and_lex_rule::match(const char* input) const {
		auto lres = m_Left->match(input);
		if (lres) {
			auto rres = m_Right->match(input + lres);
			if (rres) {
				return lres + rres;
			}
		}
		return 0;
	}
}