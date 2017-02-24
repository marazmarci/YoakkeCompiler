#pragma once

namespace yk {
	template <typename T, typename U>
	struct iterator {
	protected:
		U* m_Position;

	public:
		virtual T next() = 0;
		virtual bool has_next() = 0;

		void set_position(U* pos) {
			m_Position = pos;
		}

		U* get_position() {
			return m_Position;
		}
	};
}