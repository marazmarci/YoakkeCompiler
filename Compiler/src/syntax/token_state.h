#pragma once

#include "../common.h"

struct token_stream;
struct token;

struct token_state {
public:
	token_state(token_stream& state);

public:
	token const& head();
	token_state tail();

protected:
	token_state(token_stream& state, ysize idx);

private:
	token_stream& m_State;
	ysize m_Index;
};
