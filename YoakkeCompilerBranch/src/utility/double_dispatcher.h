#pragma once

#include "base_of.h"

namespace yk {
	// Dispatching mechanism
	template <typename T, typename... T>
	struct double_dispatcher_disp;

	template <typename T, typename Head>
	struct double_dispatcher_disp<T, Head> {
		template <typename D>
		static inline
			std::enable_if_t<!std::is_same<T, void>::value, T>
			dispatch(D* dispatcher, typename D::Base* base) {
			if (Head* sub = dynamic_cast<Head*>(base)) {
				return dispatcher->dispatch(sub);
			}
			else {
				throw std::exception("Unimplemented dispatc function for dispatcher!");
			}
		}

		template <typename D>
		static inline
			std::enable_if_t<std::is_same<T, void>::value, T>
			dispatch(D* dispatcher, typename D::Base* base) {
			if (Head* sub = dynamic_cast<Head*>(base)) {
				dispatcher->dispatch(sub);
			}
			else {
				throw std::exception("Unimplemented dispatc function for dispatcher!");
			}
		}
	};

	template <typename T, typename Head, typename... Tail>
	struct double_dispatcher_disp<T, Head, Tail...> {
		template <typename D>
		static inline 
			std::enable_if_t<!std::is_same<T, void>::value, T>
			dispatch(D* dispatcher, typename D::Base* base) {
			if (Head* sub = dynamic_cast<Head*>(base)) {
				return dispatcher->dispatch(sub);
			}
			else {
				return double_dispatcher_disp<Tail...>::dispatch(dispatcher, base);
			}
		}

		template <typename D>
		static inline
			std::enable_if_t<std::is_same<T, void>::value, T>
			dispatch(D* dispatcher, typename D::Base* base) {
			if (Head* sub = dynamic_cast<Head*>(base)) {
				dispatcher->dispatch(sub);
			}
			else {
				double_dispatcher_disp<Tail...>::dispatch(dispatcher, base);
			}
		}
	};

	// Dispatcher implementation
	template <typename T, typename... U>
	struct double_dispatcher_impl;

	template <typename T, typename Head>
	struct double_dispatcher_impl<T, Head> {
		virtual T dispatch(Head*) = 0;
	};

	template <typename T, typename Head, typename... Tail>
	struct double_dispatcher_impl<T, Head, Tail...> : public double_dispatcher_impl<T, Tail...> {
		using double_dispatcher_impl<T, Tail...>::dispatch;

		virtual T dispatch(Head*) = 0;
	};

	// Public dispatcher type
	template <typename T, typename Head, typename... Tail>
	struct double_dispatcher : public double_dispatcher_impl<T, Head, Tail...> {
		using Base = base_of(&Head::dispatch_id);
		using double_dispatcher_impl<T, Head, Tail...>::dispatch;

		std::enable_if_t<!std::is_same<T, void>, T>
			dispatch_gen(Base* b) {
			return double_dispatcher_disp<Head, Tail...>::dispatch(this, b);
		}

		std::enable_if_t<std::is_same<T, void>, T>
			dispatch_gen(Base* b) {
			double_dispatcher_disp<Head, Tail...>::dispatch(this, b);
		}
	};

	// Dispatchable
	struct dispatchable {
	public:
		void dispatch_id() { }

	private:
		virtual void nothing() { }
	};
}