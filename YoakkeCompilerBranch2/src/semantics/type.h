#pragma once

#include "../common.h"

namespace yk {
	class type_var;

	class type {
	public:
		virtual bool matches(ysptr<type> t) const		= 0;
		virtual bool contains(ysptr<type_var> t) const	= 0;
		virtual ystr to_str() const						= 0;
	};

	class type_var : public type {
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

	class type_cons : public type {
	public:
		ystr				Name;
		yvec<ysptr<type>>	Types;

	protected:
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

	class tuple_type_cons : public type_cons {
	public:
		template <typename... Ts>
		static ysptr<type_cons> create(Ts... params) {
			return std::make_shared<tuple_type_cons>(yvec<ysptr<type>>{ params... });
		}

	public:
		tuple_type_cons(yvec<ysptr<type>> const& ts);
	};

	class fn_type_cons : public type_cons {
	public:
		static ysptr<type_cons> create(ysptr<type> left, ysptr<type> right) {
			return std::make_shared<fn_type_cons>(yvec<ysptr<type>>{ left, right });
		}

	public:
		fn_type_cons(yvec<ysptr<type>> const& ts);

	public:
		virtual bool matches(ysptr<type> t) const override;
	};

	class primitive_type_cons : public type_cons {
	public:
		static ysptr<type_cons> create(ystr const& name) {
			return std::make_shared<type_cons>(name, yvec<ysptr<type>>{ });
		}
	};
}
