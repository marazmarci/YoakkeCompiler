#pragma once

#include "symbol.h"

namespace yk {
	class type_symbol : public symbol {
	protected:
		type_symbol(ystr const& id);

	public:
		virtual ~type_symbol();

	public:
		virtual bool same(yshared_ptr<type_symbol> other);
		virtual bool match(yshared_ptr<type_symbol> other);
	};

	class native_type_symbol : public type_symbol {
	public:
		native_type_symbol(ystr const& id);
		virtual ~native_type_symbol();
	};

	class fn_type_symbol : public type_symbol {
	public:
		yshared_ptr<type_symbol> Parameters;
		yshared_ptr<type_symbol> Return;

	public:
		static ystr create_name(yshared_ptr<type_symbol> p, yshared_ptr<type_symbol> r);

	public:
		fn_type_symbol(yshared_ptr<type_symbol> p, yshared_ptr<type_symbol> r);
		virtual ~fn_type_symbol();

	public:
		virtual bool same(yshared_ptr<type_symbol> other) override;
		virtual bool match(yshared_ptr<type_symbol> other) override;
	};

	class tuple_type_symbol : public type_symbol {
	public:
		yvec<yshared_ptr<type_symbol>> Elements;

	public:
		static ystr create_name(yvec<yshared_ptr<type_symbol>> const& ls);

	public:
		tuple_type_symbol(yvec<yshared_ptr<type_symbol>>& ls);
		virtual ~tuple_type_symbol();

	public:
		virtual bool same(yshared_ptr<type_symbol> other) override;
		virtual bool match(yshared_ptr<type_symbol> other) override;
	};
}
