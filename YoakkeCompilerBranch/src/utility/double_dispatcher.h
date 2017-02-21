#pragma once

namespace yk {
	// Double dispatcher mechanism
	template <typename... T>
	struct double_dispatcher_gen;

	template <typename Disp, typename Ret, typename Base, typename Head>
	struct double_dispatcher_gen<Disp, Ret, Base, Head> {
		static inline Ret dispatch(Disp* dispatcher, Base* base) {
			if (Head* sub = dynamic_cast<Head*>(base)) {
				return dispatcher->dispatch(sub);
			}
			else {
				throw std::exception("Double dispatcher unimplemented type!");
			}
		}
	};

	template <typename Disp, typename Base, typename Head>
	struct double_dispatcher_gen<Disp, void, Base, Head> {
		static inline void dispatch(Disp* dispatcher, Base* base) {
			if (Head* sub = dynamic_cast<Head*>(base)) {
				dispatcher->dispatch(sub);
			}
			else {
				throw std::exception("Double dispatcher unimplemented type!");
			}
		}
	};

	template <typename Disp, typename Ret, typename Base, typename Head, typename... Tail>
	struct double_dispatcher_gen<Disp, Ret, Base, Head, Tail...> {
		static inline Ret dispatch(Disp* dispatcher, Base* base) {
			if (Head* sub = dynamic_cast<Head*>(base)) {
				return dispatcher->dispatch(sub);
			}
			else {
				return double_dispatcher_gen<Disp, Ret, Base, Tail...>::dispatch(dispatcher, base);
			}
		}
	};

	template <typename Disp, typename Base, typename Head, typename... Tail>
	struct double_dispatcher_gen<Disp, void, Base, Head, Tail...> {
		static inline void dispatch(Disp* dispatcher, Base* base) {
			if (Head* sub = dynamic_cast<Head*>(base)) {
				dispatcher->dispatch(sub);
			}
			else {
				double_dispatcher_gen<Disp, void, Base, Tail...>::dispatch(dispatcher, base);
			}
		}
	};

	// Double dispatcher interface
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

	// Public dispatcher interface
	template <typename... T>
	struct double_dispatcher;

	template <typename Ret, typename Base, typename... Types>
	struct double_dispatcher<Ret, Base, Types...> : public double_dispatcher_int<Ret, Types...> {
		using double_dispatcher_int<Ret, Types...>::dispatch;

		inline Ret dispatch_gen(Base* b) {
			return double_dispatcher_gen<double_dispatcher<Ret, Base, Types...>, Ret, Base, Types...>
				::dispatch(this, b);
		}
	};

	template <typename Base, typename... Types>
	struct double_dispatcher<void, Base, Types...> : public double_dispatcher_int<void, Types...> {
		using double_dispatcher_int<void, Types...>::dispatch;

		inline void dispatch_gen(Base* b) {
			return double_dispatcher_gen<double_dispatcher<void, Base, Types...>, void, Base, Types...>
				::dispatch(this, b);
		}
	};

	// Visitor specialization
	template <typename... T>
	using visitor = double_dispatcher<void, T...>;
}