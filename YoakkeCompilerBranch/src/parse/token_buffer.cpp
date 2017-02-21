#include "token_buffer.h"

namespace yk {
	token_buffer::token_buffer(iterator<token>* iter)
		: m_Iter(iter) {
	}

	void token_buffer::clear() {
		m_Buffer.clear();
	}
}