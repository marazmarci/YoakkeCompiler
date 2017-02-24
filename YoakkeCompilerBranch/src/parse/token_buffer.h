#pragma once

#include "token.h"
#include "../utility/iterator.h"
#include "../common.h"
#include "../utility/optional.h"

namespace yk {
	class token_buffer {
	private:
		friend class parser;

	private:
		iterator<token, const char>* m_Iter;
		yvec<token> m_Buffer;

	public:
		token_buffer(iterator<token, const char>* iter);

	public:
		void clear();
	};
}