#pragma once

#include <typeinfo>
#include <type_traits>
#include <utility>

#include "static_contains.h"
#include "static_max.h"
#include "../common.h"

namespace yk {
	template <typename... Types>
	struct variant_helper;

	template <typename Head, typename... Tail>
	struct variant_helper<Head, Tail...> {
		static inline void destruct(ysize id, void* data) {
			if (id == typeid(Head).hash_code()) {
				reinterpret_cast<Head*>(data)->~Head();
			}
			else {
				variant_helper<Tail...>::destruct(id, data);
			}
		}

		static inline void move(ysize prevt, void* prevv, void* newv) {
			if (prevt == typeid(Head).hash_code()) {
				new (newv) Head(std::move(*reinterpret_cast<Head*>(prevv)));
			}
			else {
				variant_helper<Tail...>::move(prevt, prevv, newv);
			}
		}

		static inline void copy(ysize prevt, const void* prevv, void* newv) {
			if (prevt == typeid(Head).hash_code()) {
				new (newv) Head(*reinterpret_cast<const Head*>(prevv));
			}
			else {
				variant_helper<Tail...>::copy(prevt, prevv, newv);
			}
		}
	};

	template<>
	struct variant_helper<> {
		static inline void destruct(ysize id, void* data) { }
		static inline void move(ysize prevt, void* prevv, void* newv) { }
		static inline void copy(ysize prevt, const void* prevv, void* newv) { }
	};

	template <typename... Types>
	class variant {
	private:
		static const ysize data_size =		static_max<ysize, sizeof(Types)...>::value;
		static const ysize data_alignment = static_max<ysize, alignof(Types)...>::value;

		using data_t = std::aligned_storage_t<data_size, data_alignment>;
		using helper_t = variant_helper<Types...>;

	private:
		ysize type_id;
		data_t data;

	public:
		variant() 
			: type_id(invalid_type()) {
		}

		variant(const variant<Types...>& prev)
			: type_id(prev.type_id) {
			helper_t::copy(prev.type_id, &prev.data, &data);
		}

		variant(variant<Types...>&& prev) 
			: type_id(prev.type_id) {
			helper_t::move(prev.type_id, &prev.data, &data);
		}

		~variant() {
			helper_t::destruct(type_id, &data);
		}

	public:
		variant<Types...>& operator=(variant<Types...> prev) {
			std::swap(type_id, prev.type_id);
			std::swap(data, prev.data);
			return *this;
		}

		template <typename T>
		std::enable_if_t<static_contains<T, Types...>::value, T const&>
		operator=(T const& other) {
			this->set<T>(other);
			return other;
		}

		template <typename T>
		std::enable_if_t<static_contains<T, Types...>::value, bool>
		is() const {
			return type_id == typeid(T).hash_code();
		}

		bool valid() const {
			return type_id != invalid_type();
		}

		template<typename T, typename... Args>
		std::enable_if_t<static_contains<T, Types...>::value>
		set(Args&&... args) {
			helper_t::destruct(type_id, &data);
			new (&data) T(std::forward<Args>(args)...);
			type_id = typeid(T).hash_code();
		}

		template<typename T>
		std::enable_if_t<static_contains<T, Types...>::value, T&>
		get() {
			if (type_id == typeid(T).hash_code()) {
				return *reinterpret_cast<T*>(&data);
			}
			else {
				throw std::bad_cast();
			}
		}

	private:
		static inline ysize invalid_type() {
			return typeid(void).hash_code();
		}
	};

	template <typename... Ts>
	using yvar = variant<Ts...>;
}