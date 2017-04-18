#pragma once

#include "symbol.h"

namespace yk {
	class type_symbol : public symbol {
	protected:
		type_symbol(ystr const& id);

	public:
		virtual ~type_symbol();

	public:
		virtual bool same(ysptr<type_symbol> other);
		virtual bool match(ysptr<type_symbol> other);
	};

	class native_type_symbol : public type_symbol {
	public:
		native_type_symbol(ystr const& id);
		virtual ~native_type_symbol();
	};

	class fn_type_symbol : public type_symbol {
	public:
		ysptr<type_symbol> Parameters;
		ysptr<type_symbol> Return;

	public:
		static ystr create_name(ysptr<type_symbol> p, ysptr<type_symbol> r);

	public:
		fn_type_symbol(ysptr<type_symbol> p, ysptr<type_symbol> r);
		virtual ~fn_type_symbol();

	public:
		virtual bool same(ysptr<type_symbol> other) override;
		virtual bool match(ysptr<type_symbol> other) override;
	};

	class tuple_type_symbol : public type_symbol {
	public:
		yvec<ysptr<type_symbol>> Elements;

	public:
		static ystr create_name(yvec<ysptr<type_symbol>> const& ls);

	public:
		tuple_type_symbol(yvec<ysptr<type_symbol>>& ls);
		virtual ~tuple_type_symbol();

	public:
		virtual bool same(ysptr<type_symbol> other) override;
		virtual bool match(ysptr<type_symbol> other) override;
	};
}
