#pragma once

#include "../common.h"

namespace yk {
	class file_handle {
	private:
		ystr m_Name;
		char* m_Src;
		yvec<const char*> m_Lines;

	public:
		file_handle(ystr const& file);
		~file_handle();

	public:
		const char* ptr() const;
		const char* line(ysize num) const;
		ystr const& name() const;
	};
}
