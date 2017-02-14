#pragma once

#include <stdexcept>

namespace yk {
	struct NoneT {};
	extern NoneT None;

	template <typename T>
	class optional {
	private:
		bool flag;
		union { T data; };

	public:
		optional()
			: flag(false) {
		}

		optional(T const& val)
			: flag(true), data(val) {
		}

		optional(NoneT const& val)
			: flag(false) {
		}

		~optional() {
			destruct();
		}

	public:
		optional<T>& operator=(T const& el) {
			destruct();
			flag = true;
			data = el;
		}

		optional<T>& operator=(NoneT const& el) {
			destruct();
			flag = false;
		}

		optional<T>& operator=(optional<T> prev) {
			std::swap(flag, prev.flag);
			std::swap(data, prev.data);
			return *this;
		}

		inline bool contains() const {
			return flag;
		}

		inline bool empty() const {
			return !flag;
		}

		T& get() {
			if (flag) {
				return data;
			}
			else {
				throw std::exception("Tried to fetch optional<T> without actual data!");
			}
		}

	private:
		void destruct() {
			if (flag) {
				data.~T();
			}
		}
	};

	template <typename T>
	using yopt = optional<T>;
}