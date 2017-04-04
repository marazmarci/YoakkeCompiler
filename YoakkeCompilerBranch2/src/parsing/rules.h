#pragma once

#include "../lexing/token.h"
#include "parse_error.h"

#define throw_expect(x, par)				\
{ token const& m_tok_ = (par).peek();		\
throw expect_parse_err(x,					\
	yparser::format_token(m_tok_),			\
	(par).file(), m_tok_.Position); }
