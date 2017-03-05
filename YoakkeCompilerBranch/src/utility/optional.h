#pragma once

#include <stdexcept>

namespace yk {
	class none_t { };
	extern none_t None;

	template <typename T>
	class optional {
	private:
		bool m_Flag;
		union { T m_Data; };

	public:
		optional() 
			: m_Flag(false) {
		}

		optional(none_t n)
			: m_Flag(false) {
		}

		optional(T const& d)
			: m_Flag(true) {
			new (&m_Data) T(d);
		}

		optional(optional<T> const& opt)
			: m_Flag(opt.m_Flag) {
			if (m_Flag) {
				new (&m_Data) T(opt.m_Data);
			}
		}

		~optional() {
			destruct();
		}

	public:
		optional<T> const& operator=(optional<T> const& other) {
			m_Flag = other.m_Flag;
			if (m_Flag) {
				new (&m_Data) T(other.m_Data);
			}
			return *this;
		}

		optional<T> const& operator=(none_t other) {
			destruct();
			m_Flag = false;
			return *this;
		}

		T& get() {
			if (m_Flag) {
				return m_Data;
			}
			else {
				throw std::exception("Tried to get optional value without value");
			}
		}

		bool some() {
			return m_Flag;
		}

		bool none() {
			return !m_Flag;
		}

	private:
		void destruct() {
			if (m_Flag) {
				m_Data.~T();
				m_Flag = false;
			}
		}
	};

	template <typename T>
	using yopt = optional<T>;
}