#pragma once

#include <vector>
#include <cstddef>

namespace yk {
	// Counter mechanism
	template <typename Base>
	struct visitor_tag_counter {
		static std::size_t s_Counter;
	};

	template <typename Base>
	std::size_t visitor_tag_counter<Base>::s_Counter = 0;

	// Container for the unique tag
	template <typename Visitable, typename Base>
	struct visitable_tag_container {
		static std::size_t s_Tag;
	};

	// Helper to register or get the tag if registered already
	template <typename Visitable, typename Base>
	std::size_t visitable_get_tag() {
		std::size_t& tag = visitable_tag_container<Visitable const, Base const>::s_Tag;
		return tag == 0 ? (tag = ++visitor_tag_counter<Base const>::s_Counter) : tag;
	}

	template <typename Visitable, typename Base>
	std::size_t
		visitable_tag_container<Visitable, Base>::s_Tag = visitable_get_tag<Visitable, Base>();

	template <typename Base, typename Func>
	class visitor_vtable {
	private:
		std::vector<Func> m_Table;
		std::vector<bool> m_Status;

	public:
		template <typename Visitable>
		void add(Func f) {
			std::size_t index = visitable_get_tag<Visitable, Base>();
			if (index >= m_Table.size()) {
				std::size_t const base_tag = visitable_get_tag<Base, Base>();

				Func defaultFunc = (base_tag >= m_Table.size()) ? nullptr : m_Table[base_tag];

				m_Table.resize(index + 1, defaultFunc);
				m_Status.resize(index + 1, false);
			}

			m_Table[index] = f;
			m_Status[index] = true;
		}

		Func operator[](std::size_t index) const {
			if (index >= m_Table.size()) {
				index = visitable_get_tag<Base, Base>();
			}
			return m_Table[index];
		}

		std::vector<bool> const& get_status_table() const {
			return m_Status;
		}
	};

	template <typename... T>
	struct visitor_thunk_tag { };

	template <>
	struct visitor_thunk_tag<> { };

	template <typename Visitor, typename Invoker, typename... VisitedList>
	struct visitor_get_vtable;

	template <typename Visitor, typename Invoker, typename... VisitedList>
	class visitor_vtable_creator {
	private:
		friend struct visitor_get_vtable<Visitor, Invoker, VisitedList...>;

	private:
		typename Visitor::vtable_t m_Vtable;

	private:
		visitor_vtable_creator() {
			add_thunk<typename Visitor::base_t>();
			add_thunks(visitor_thunk_tag<VisitedList...>());
		}

	public:
		template <typename Visitable>
		void add_thunk() {
			m_Vtable.template add<Visitable>(&Visitor::template thunk<Visitor, Visitable, Invoker>);
		}

		template <typename Head, typename... Tail>
		void add_thunks(visitor_thunk_tag<Head, Tail...>) {
			add_thunk<Head>();
			add_thunks(visitor_thunk_tag<Tail...>());
		}

		void add_thunks(visitor_thunk_tag<>) { }
	};

	template <typename Visitor, typename Invoker, typename... VisitedList>
	struct visitor_get_vtable {
		static visitor_vtable_creator<Visitor, Invoker, VisitedList...> const s_Table;

		operator typename Visitor::vtable_t const *() const {
			return &s_Table.m_Vtable;
		}
	};

	template <typename Visitor, typename Invoker, typename... VisitedList>
	visitor_vtable_creator<Visitor, Invoker, VisitedList...> const
		visitor_get_vtable<Visitor, Invoker, VisitedList...>::s_Table;

	struct visit_invocation_info {
		std::size_t VtableIndex;
		void* Visitable;

		visit_invocation_info(std::size_t index, void* v)
			: VtableIndex(index), Visitable(v) {
		}
	};

	template <typename Base>
	struct visitable {
	protected:
		template <typename VisitableImpl>
		std::size_t get_tag_helper(VisitableImpl const*) const;
	};

#define META_Visitable(VisitableImpl, VisitableFallback)				\
virtual visit_invocation_info											\
visitable_invocation_info(std::vector<bool> const& statusTable) {		\
	std::size_t tag = get_tag_helper(this);								\
	if(tag < statusTable.size() && statusTable[tag]) {					\
		return visit_invocation_info(tag, this);						\
	}																	\
	return VisitableFallback::visitable_invocation_info(statusTable);	\
}

#define META_BaseVisitable(VisitableImpl) META_Visitable(VisitableImpl, VisitableImpl)

	template <typename Base>
	template <typename VisitableImpl>
	inline std::size_t visitable<Base>::get_tag_helper(VisitableImpl const*) const {
		return visitable_get_tag<VisitableImpl, Base>();
	}

	template <typename Base, typename ReturnType = void, typename... Args>
	class visitor {
	public:
		using base_t = Base;
		using thunk_t = ReturnType(visitor::*)(Base&, Args&&...);
		using vtable_t = visitor_vtable<Base const, thunk_t>;
		using return_t = ReturnType;

	private:
		template <typename VisitorImpl, typename Invoker, typename... VisitedList>
		friend struct visitor_vtable_setter;

	private:
		vtable_t const* m_Vtable;

	public:
		ReturnType operator()(Base& b, Args&&... args);

		template <typename VisitorImpl, typename Visitable, typename Invoker>
		ReturnType thunk(Base& b, Args&&... args);
	};

#define META_Visitor(...) _SELECT_META_VISITOR_(__VA_ARGS__)

#define META_Visitables(...)					\
    visitor_vtable_setter<						\
        visitor_invoker_details::VisitorType,	\
        visitor_invoker_details::InvokerType,	\
        __VA_ARGS__								\
    >::set_vtable(*this)

	template <typename Visitable, typename Base>
	struct visit_method_get_arg_t {
		using type_t = Visitable;
	};

	template <typename Visitable, typename Base>
	struct visit_method_get_arg_t<Visitable, Base const> {
		using type_t = Visitable const;
	};


	template <typename Base, typename ReturnType, typename... Args>
	inline ReturnType visitor<Base, ReturnType, Args...>::operator()(Base& b, Args&&... args) {
		auto info = b.visitable_invocation_info(m_Vtable->get_status_table());

		thunk_t thunk = (*m_Vtable)[info.VtableIndex];
		return (this->*thunk)(*static_cast<Base *>(info.Visitable), std::forward<Args>(args)...);
	}

	template <typename Base, typename ReturnType, typename... Args>
	template <typename VisitorImpl, typename Visitable, typename Invoker>
	inline ReturnType visitor<Base, ReturnType, Args...>::thunk(Base& b, Args&&... args) {
		using VisitableType = typename visit_method_get_arg_t<Visitable, Base>::type_t;

		VisitorImpl& visitor = static_cast<VisitorImpl&>(*this);
		VisitableType& Visitable = static_cast<VisitableType&>(b);
		return Invoker::invoke(visitor, Visitable, std::forward<Args>(args)...);
	}

	template <typename Visitor, typename Invoker, typename... VisitedList>
	struct visitor_vtable_setter {
		static void set_vtable(Visitor& visitor) {
			visitor.m_Vtable = visitor_get_vtable<Visitor, Invoker, VisitedList...>();
		}
	};

#define _META_VISITOR_DEFAULT_(VisitorImpl) _META_VISITOR_WITH_CUSTOM_INVOKER_(VisitorImpl, visit)

#define _META_VISITOR_WITH_CUSTOM_INVOKER_(VisitorImpl, VisitInvoker)				\
struct visitor_invoker_details {													\
	using VisitorType = VisitorImpl;												\
	using InvokerType =																\
	struct {																		\
		template <typename VisitorImpl, typename VisitableImpl, typename... Args>	\
		static typename VisitorImpl::return_t invoke(								\
			VisitorImpl& visitor, VisitableImpl& Visitable, Args&&... args) {		\
			return visitor.VisitInvoker(Visitable, std::forward<Args>(args)...);	\
		}																			\
	};																				\
};

#define _META_VISITOR_1_(VisitorImpl) \
	_META_VISITOR_DEFAULT_(VisitorImpl)

#define _META_VISITOR_2_(VisitorImpl, VisitInvoker) \
	_META_VISITOR_WITH_CUSTOM_INVOKER_(VisitorImpl, VisitInvoker)

#define _META_VISITOR_N_ARGS_(...) _META_VISITOR_N_ARGS__IMPL_((__VA_ARGS__, 2, 1, 0))
#define _META_VISITOR_N_ARGS__IMPL_(args) _META_VISITOR_N_ARGS__IMPL_2_ args
#define _META_VISITOR_N_ARGS__IMPL_2_(_1, _2, count, ...) count

#define _META_VISITOR_CHOOSER_(count)  _META_VISITOR_CHOOSER_1_(count)
#define _META_VISITOR_CHOOSER_1_(count) _META_VISITOR_CHOOSER_2_(count)
#define _META_VISITOR_CHOOSER_2_(count) _META_VISITOR_##count##_

#define _SELECT_META_VISITOR_HELPER_(func, args) func args
#define _SELECT_META_VISITOR_(...) \
	_SELECT_META_VISITOR_HELPER_(\
	_META_VISITOR_CHOOSER_(_META_VISITOR_N_ARGS_(__VA_ARGS__)), (__VA_ARGS__))
}
