#include <stdio.h>
#include "file_handle.h"

namespace yk {
	file_handle::file_handle(ystr const& file)
		: m_Name(file) {
		FILE* f = fopen(file.c_str(), "r");
		if (f) {
			fseek(f, 0, SEEK_END);
			ysize length = ftell(f);
			m_Src = new char[length + 2];
			fseek(f, 0, SEEK_SET);

			ysize line_len = 0;
			char* ptr = m_Src;
			char c;
			m_Lines.push_back(ptr);
			while ((c = fgetc(f)) != EOF) {
				*ptr++ = c;
				line_len++;
				if (c == '\n') {
					m_Lines.push_back(ptr);
					m_LineLengths.push_back(line_len - 1);
					line_len = 0;
				}
			}
			m_LineLengths.push_back(line_len - 1);
			*ptr++ = '\n';
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

	ysize file_handle::line_len(ysize num) const {
		return m_LineLengths[num];
	}

	ystr const& file_handle::name() const {
		return m_Name;
	}
}
