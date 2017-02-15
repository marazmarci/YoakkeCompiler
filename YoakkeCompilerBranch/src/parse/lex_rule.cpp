#include <cstring>

#include "lex_rule.h"

namespace yk {
	namespace lex_rule {
		// Generic rule
		rule::rule(code c) 
			: m_Hash(c), Next(&END) {
		}

		rule::~rule() { }

		code rule::hash_code() const {
			return m_Hash;
		}

		rule& operator+(rule & l, rule & r) {
			l.Next = &r;
			return l;
		}

		// Rule end
		end::end() 
			: rule(code::end) {
		}

		end::~end() { }

		// Char sequence
		chars::chars(ystr const& seq) 
			: rule(code::chars), Sequence(seq) {
		}

		chars::~chars() { }

		// Multiple occurences
		mul::mul(rule& sub)
			: rule(code::mul), Sub(&sub) {
			Sub->Next = this;
		}

		mul::~mul() {
			delete Sub;
		}

		// Optional
		opt::opt(rule& sub) 
			: rule(code::opt), Sub(&sub) {
			Sub->Next = &END;
		}

		opt::~opt() {
			delete Sub;
		}

		end END;

		// Matching function
#define EAT_STACK() {if (stack.size()) {		\
						auto top = stack.top();	\
						offset = top.second;	\
						begin = top.first;		\
						stack.pop();			\
					}							\
					else {						\
						return longest;			\
					}}

		ysize match(const char* str, rule& thebegin) {
			rule* begin = &thebegin;
			ysize longest = 0;
			ysize offset = 0;
			ystack<ypair<rule*, ysize>> stack;

			for (;;) {
				switch (begin->hash_code()) {
				case code::end:
					if (offset > longest) {
						longest = offset;
					}
					EAT_STACK();
					break;

				case code::chars: {
					chars* ch = reinterpret_cast<chars*>(begin);
					ysize len = ch->Sequence.size();
					if (std::strncmp(str + offset, ch->Sequence.c_str(), len) == 0) {
						offset += len;
						begin = ch->Next;
					}
					else {
						EAT_STACK();
					}
					break;
				}

				case code::mul: {
					mul* ml = reinterpret_cast<mul*>(begin);
					stack.push(std::make_pair(ml->Next, offset));
					begin = ml->Sub;
					break;
				}

				case code::opt: {
					opt* op = reinterpret_cast<opt*>(begin);
					stack.push(std::make_pair(op->Next, offset));
					begin = op->Sub;
					break;
				}

				default:
					throw std::exception("Lex match rule unimplemented!");
				}
			}
			return 0;
		}
	}
}