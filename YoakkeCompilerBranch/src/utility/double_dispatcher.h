#pragma once

#include "base_of.h"

namespace yk {
	// Dispatching mechanism
	template <typename... T>
	struct double_dispatcher_gen;

	template <typename Disp, typename Ret, typename Head>
	struct double_dispatcher_gen<Disp, Ret, Head> {
		static inline Ret dispatch(Disp* dispatcher, typename Disp::Base* base) {
			if (Head* sub = dynamic_cast<Head*>(base)) {
				return dispatcher->dispatch(sub);
			}
			else {
				throw std::exception("Unimplemented dispatc function for dispatcher!");
			}
		}
	};

	template <typename Disp, typename Head>
	struct double_dispatcher_gen<Disp, void, Head> {
		static inline void dispatch(Disp* dispatcher, typename Disp::Base* base) {
			if (Head* sub = dynamic_cast<Head*>(base)) {
				dispatcher->dispatch(sub);
			}
			else {
				throw std::exception("Unimplemented dispatc function for dispatcher!");
			}
		}
	};

	template <typename Disp, typename Ret, typename Head, typename... Tail>
	struct double_dispatcher_gen<Disp, Ret, Head, Tail...> {
		static inline Ret dispatch(Disp* dispatcher, typename Disp::Base* base) {
			if (Head* sub = dynamic_cast<Head*>(base)) {
				return dispatcher->dispatch(sub);
			}
			else {
				return double_dispatcher_gen<Disp, Ret, Tail...>::dispatch(dispatcher, base);
			}
		}
	};

	template <typename Disp, typename Head, typename... Tail>
	struct double_dispatcher_gen<Disp, void, Head, Tail...> {
		static inline void dispatch(Disp* dispatcher, typename Disp::Base* base) {
			if (Head* sub = dynamic_cast<Head*>(base)) {
				dispatcher->dispatch(sub);
			}
			else {
				double_dispatcher_gen<Disp, void, Tail...>::dispatch(dispatcher, base);
			}
		}
	};

	// Interface implementation
	template <typename... T>
	struct double_dispatcher_int;

	template <typename Ret, typename Head>
	struct double_dispatcher_int<Ret, Head> {
		virtual Ret dispatch(Head*) = 0;
	};

	template <typename Ret, typename Head, typename... Tail>
	struct double_dispatcher_int<Ret, Head, Tail...> : public double_dispatcher_int<Ret, Tail...> {
		using double_dispatcher_int<Ret, Tail...>::dispatch;

		virtual Ret dispatch(Head*) = 0;
	};

	// Public interface for double dispatcher
	template <typename... T>
	struct double_dispatcher;

	template <typename Ret, typename Head, typename... Tail>
	struct double_dispatcher<Ret, Head, Tail...> : public double_dispatcher_int<Ret, Head, Tail...> {
		using Base = base_of(&Head::dispatch_id);
		using double_dispatcher_int<Ret, Head, Tail...>::dispatch;

		template <typename U>
		Ret dispatch_gen(U* b) {
			return double_dispatcher_gen<double_dispatcher<Ret, Head, Tail...>, Ret, Head, Tail...>
				::dispatch(this, (Base*)(b));
		}
	};

	template <typename Head, typename... Tail>
	struct double_dispatcher<void, Head, Tail...> : public double_dispatcher_int<void, Head, Tail...> {
		using Base = base_of(&Head::dispatch_id);
		using double_dispatcher_int<void, Head, Tail...>::dispatch;

		template <typename U>
		void dispatch_gen(U* b) {
			double_dispatcher_gen<double_dispatcher<void, Head, Tail...>, void, Head, Tail...>
				::dispatch(this, (Base*)(b));
		}
	};

	// Visitor specialization
	template <typename... T>
	using visitor = double_dispatcher<void, T...>;

	// Dispatchable
	struct double_dispatchable {
	public:
		void dispatch_id() { }

	private:
		virtual void nothing() { }
	};
}