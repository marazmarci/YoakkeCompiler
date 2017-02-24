#include <cctype>
#include <cstring>

#include "ylexer.h"

namespace yk {
	namespace lex_rules {
		static ysize whitespace(const char* src) {
			if (*src == ' ' || *src == '\t' || *src == '\n') {
				return 1;
			}
			else {
				return 0;
			}
		}

		static ysize single_line_comment(const char* src) {
			if (*src++ == '/' && *src++ == '/') {
				ysize skip = 2;
				while (*src && *src++ != '\n') skip++;
				return skip;
			}
			else {
				return 0;
			}
		}

		static ysize multi_line_comment(const char* src) {
			if (*src++ == '/' && *src++ == '*') {
				ysize skip = 2;
				ysize depth = 1;
				while (*src && depth) {
					if (*src == '/' && *(src + 1) == '*') {
						src += 2;
						skip += 2;
						depth++;
					}
					else if (*src == '*' && *(src + 1) == '/') {
						src += 2;
						skip += 2;
						depth--;
					}
					else {
						src++;
						skip++;
					}
				}
				return skip;
			}
			else {
				return 0;
			}
		}

		typedef yopt<ypair<ystr, ysize>> token_desc;

		static const yset<ystr> keywords = {
			"if", "while", "fn"
		};

		static token_desc identifier(const char* src) {
			ysize skip = 0;
			if (std::isalpha(*src) || *src == '_') {
				while (std::isalnum(*src) || *src == '_') {
					*src++;
					skip++;
				}
				ystr ident = ystr(src - skip, src);
				if (keywords.find(ystr(src - skip, src)) != keywords.end()) {
					return std::make_pair(ident, skip);
				}
				else {
					return std::make_pair("Identifier", skip);
				}
			}
			else {
				return None;
			}
		}

		static token_desc numeric_literal(const char* src) {
			ysize skip = 0;
			if (std::isdigit(*src)) {
				while (std::isdigit(*src)) src++, skip++;
			}
			if (*src == '.' && (skip || std::isdigit(*(src + 1)))) {
				src++, skip++;
				while (std::isdigit(*src++)) skip++;
				return std::make_pair("Real", skip);
			}
			else if (skip) {
				return std::make_pair("Integer", skip);
			}
			else {
				return None;
			}
		}

		static const yset<ystr> symbols = {
			"(", ")",
			"::",
			",",
			"=",
			"||", "&&",
			"==", "<>",
			">", "<", ">=", "<=",
			"+", "-", "*", "/", "%",
			"!", ".", "->",
			":",
			"{", "}",
			";"
		};

		static token_desc symbol(const char* src) {
			for (auto it = symbols.rbegin(); it != symbols.rend(); it++) {
				auto& sym = *it;
				if (std::strncmp(sym.c_str(), src, sym.length()) == 0) {
					return std::make_pair(sym, sym.length());
				}
			}
			return None;
		}
	}

	ylexer::ylexer() {
		// Add skip sequences
		add_skip(lex_rules::whitespace);
		add_skip(lex_rules::single_line_comment);
		add_skip(lex_rules::multi_line_comment);

		// Actual tokenization
		add_rule(lex_rules::identifier);
		add_rule(lex_rules::numeric_literal);
		add_rule(lex_rules::symbol);
	}
}