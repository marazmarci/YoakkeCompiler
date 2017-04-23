#include <stdio.h>
#include "file_handle.h"

namespace yk {
	file_handle::file_handle(ystr const& file)
		: m_Name(file) {
		FILE* f = fopen(file.c_str(), "r");
		if (f) {
			fseek(f, 0, SEEK_END);
			ysize length = ftell(f);
			m_Src = new char[length + 1];
			fseek(f, 0, SEEK_SET);

			char* ptr = m_Src;
			char c;
			m_Lines.push_back(ptr);
			while ((c = fgetc(f)) != EOF) {
				*ptr++ = c;
				if (c == '\n') {
					m_Lines.push_back(ptr);
				}
			}
			*ptr = '\0';
		}
		else {
			// TODO: which file?
			throw std::exception("Could not open file!");
		}
	}

	file_handle::~file_handle() {
		delete[] m_Src;
	}

	const char* file_handle::ptr() const {
		return m_Src;
	}

	const char* file_handle::line(ysize num) const {
		return m_Lines[num];
	}

	ystr const& file_handle::name() const {
		return m_Name;
	}
}
