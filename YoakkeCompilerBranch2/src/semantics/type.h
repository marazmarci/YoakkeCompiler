#pragma once

#include "../common.h"

namespace yk {
	struct type_var;

	struct type {
	public:
		virtual bool matches(ysptr<type> t) const		= 0;
		virtual bool contains(ysptr<type_var> t) const	= 0;
		virtual ystr to_str() const						= 0;
	};

	struct type_var : public type {
	private:
		static ysize s_IDcount;

	public:
		ysize		ID;
		ysptr<type> Instance;

	public:
		static ysptr<type_var> create() {
			return std::make_shared<type_var>();
		}

	public:
		type_var();

	public:
		virtual bool matches(ysptr<type> t) const override;
		virtual bool contains(ysptr<type_var> t) const override;
		virtual ystr to_str() const override;
	};

	struct type_cons : public type {
	public:
		ystr				Name;
		yvec<ysptr<type>>	Types;

	public:
		template <typename... Ts>
		static ysptr<type_cons> create(ystr const& n, Ts... params) {
			return std::make_shared<type_cons>(n, yvec<ysptr<type>>{ params... });
		}

	public:
		type_cons(ystr const& n, yvec<ysptr<type>> const& ts);

	public:
		virtual bool matches(ysptr<type> t) const override;
		virtual bool contains(ysptr<type_var> t) const override;
		virtual ystr to_str() const override;
	};
}
