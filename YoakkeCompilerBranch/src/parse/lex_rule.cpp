#include <cstring>

#include "lex_rule.h"

namespace yk {
	lex_rule::lex_rule()
		: m_Next(nullptr) {
	}

	lex_rule::~lex_rule() { }

	void lex_rule::set_next(lex_rule* nxt) {
		m_Next = nxt;
	}

	or_lex_rule::or_lex_rule(lex_rule* l, lex_rule* r)
		: m_Left(l), m_Right(r) {
	}

	or_lex_rule::~or_lex_rule() {
		delete m_Left;
		delete m_Right;
	}

	ysize or_lex_rule::match(const char* input) const {
		auto res = m_Left->match(input);
		auto res2 = m_Right->match(input);
		if (res || res2) {
			if (m_Next) {
				auto nres = m_Next->match(input + res);
				auto nres2 = m_Next->match(input + res2);
				if (nres) {
					return res + nres;
				}
				else if (nres2) {
					return res2 + nres2;
				}
				else {
					return 0;
				}
			}
			else {
				if (res > res2) {
					return res;
				}
				else {
					return res2;
				}
			}
		}
		return 0;
	}

	seq_lex_rule::seq_lex_rule(ystr const& seq)
		: m_Sequence(seq) {
	}

	seq_lex_rule::~seq_lex_rule() { }

	ysize seq_lex_rule::match(const char* input) const {
		auto len = m_Sequence.length();
		if (std::strncmp(input, m_Sequence.c_str(), len) == 0) {
			if (m_Next) {
				auto nres = m_Next->match(input + len);
				if (nres) {
					return len + nres;
				}
				else {
					return 0;
				}
			}
			else {
				return len;
			}
		}
		else {
			return 0;
		}
	}

	mul_lex_rule::mul_lex_rule(lex_rule* any) 
		: m_Any(any) {
	}

	mul_lex_rule::~mul_lex_rule() {
		delete m_Any;
	}
	
	ysize mul_lex_rule::match(const char* input) const {
		ysize offset = 0;
		if (m_Next) {
			for (;;) {
				auto nmatch = m_Next->match(input + offset);
				if (nmatch) {
					return nmatch + offset;
				}
				auto res = m_Any->match(input + offset);
				if (res) {
					offset += res;
				}
				else {
					return 0;
				}
			}
		}
		else {
			for (;;) {
				auto res = m_Any->match(input + offset);
				if (res) {
					offset += res;
				}
				else {
					return offset;
				}
			}
		}
	}

	char_lex_rule::char_lex_rule(ystr const& chars) {
		for (char c : chars) {
			m_Chars.insert(c);
		}
	}

	char_lex_rule::char_lex_rule(std::initializer_list<char> list) {
		for (char c : list) {
			m_Chars.insert(c);
		}
	}

	char_lex_rule::~char_lex_rule() { }

	ysize char_lex_rule::match(const char* input) const {
		char curr = *input;
		if (m_Chars.find(curr) != m_Chars.end()) {
			if (m_Next) {
				auto nres = m_Next->match(input + 1);
				if (nres) {
					return 1 + nres;
				}
				else {
					return 0;
				}
			}
			else {
				return 1;
			}
		}
		else {
			return 0;
		}
	}

	chrange_lex_rule::chrange_lex_rule(char f, char l)
		: m_First(f), m_Last(l) {
	}

	chrange_lex_rule::~chrange_lex_rule() { }

	ysize chrange_lex_rule::match(const char* input) const {
		char curr = *input;
		if (curr >= m_First && curr <= m_Last) {
			if (m_Next) {
				auto nres = m_Next->match(input + 1);
				if (nres) {
					return 1 + nres;
				}
				else {
					return 0;
				}
			}
			else {
				return 1;
			}
		}
		else {
			return 0;
		}
	}

	namespace lr {
		lex_rule* match(ystr const& str) {
			return new seq_lex_rule(str);
		}

		lex_rule* mul(lex_rule* r) {
			return new mul_lex_rule(r);
		}

		lex_rule* set(ystr const& chars) {
			return new char_lex_rule(chars);
		}

		lex_rule* range(char l, char r) {
			return new chrange_lex_rule(l, r);
		}

		lex_rule* or(std::initializer_list<lex_rule*> elems) {
			yvec<lex_rule*> rules;
			for (auto e : elems) {
				rules.push_back(e);
			}
			if (rules.size() == 0) {
				return nullptr;
			}
			while (rules.size() > 1) {
				auto o = new or_lex_rule(rules[0], rules[1]);
				rules.erase(rules.begin(), rules.begin() + 2);
				if (!rules.size()) {
					return o;
				}
				else {
					rules.insert(rules.begin(), o);
				}
			}
		}
	}
}