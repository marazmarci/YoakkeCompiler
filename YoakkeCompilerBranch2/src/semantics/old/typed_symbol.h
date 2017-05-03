#pragma once

#include "../common.h"
#include "symbol.h"
#include "../lexing/token.h"
#include "../lexing/ytoken_t.h"

namespace yk {
	class type_symbol;

	class typed_symbol : public symbol {
	public:
		ysptr<type_symbol> Type;

	protected:
		typed_symbol(ystr const& id, ysptr<type_symbol> ty);

	public:
		virtual ~typed_symbol();
	};

	class const_typed_symbol : public typed_symbol {
	public:
		static ystr create_preury_op_name(ytoken_t op);
		static ystr create_postury_op_name(ytoken_t op);
		static ystr create_bin_op_name(ytoken_t op);

	public:
		const_typed_symbol(ystr const& id, ysptr<type_symbol> ty);
		virtual ~const_typed_symbol();
	};

	class builtin_typed_symbol : public const_typed_symbol {
	public:
		static ysptr<builtin_typed_symbol> create_preury_op(ytoken_t op, ysptr<type_symbol> ty);
		static ysptr<builtin_typed_symbol> create_postury_op(ytoken_t op, ysptr<type_symbol> ty);
		static ysptr<builtin_typed_symbol> create_bin_op(ytoken_t op, ysptr<type_symbol> ty);

	public:
		builtin_typed_symbol(ystr const& id, ysptr<type_symbol> ty);
		virtual ~builtin_typed_symbol();
	};

	class var_typed_symbol : public typed_symbol {
	public:
		var_typed_symbol(ystr const& id, ysptr<type_symbol> ty);
		virtual ~var_typed_symbol();
	};
}
