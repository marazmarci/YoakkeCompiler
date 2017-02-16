#pragma once

#include "parser.h"
#include "prefix_parselet.h"
#include "infix_parselet.h"

namespace yk {
	template <typename T>
	class prec_parser : public parser {
	private:
		typedef yset<prefix_parselet<T>*> prefix_set;
		typedef yset<infix_parselet<T>*> infix_set;

	private:
		ymap<ystr, prefix_set> m_PrefixVal;

	public:
		prec_parser(token_buffer* buff)
			: parser(buff) {
		}

	public:

	};
}