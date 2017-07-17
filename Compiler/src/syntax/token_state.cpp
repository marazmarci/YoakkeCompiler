#include "token_state.h"
#include "token_stream.h"

token_state::token_state(token_stream& state)
	: m_State(state), m_Index(0) {
}

token_state::token_state(token_stream& state, ysize idx)
	: m_State(state), m_Index(idx) {
}

token const& token_state::head() {
	return m_State.peek();
}

token_state token_state::tail() {
	return token_state(m_State, m_Index + 1);
}
