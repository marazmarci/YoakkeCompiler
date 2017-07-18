#include <cassert>
#include "file_hnd.h"

namespace {
	/**
	 * This helper function counts the number of characters from a given
	 * position until the end of line or end of string. The end of line
	 * character is included in the size.
	 * @see file_hnd
	 */
	ysize line_len(const char* src) {
		ysize len = 0;
		for (; *src; src++) {
			len++;
			if (*src == '\n') {
				break;
			}
		}
		return len;
	}
}

file_hnd::file_hnd(const char* path)
	: m_Path(path), m_Buffer(nullptr) {
	// Open file for reading
	FILE* file = std::fopen(path, "rb");
	if (!file) {
		// Could not open the file
		return;
	}
	// Go to the end of the file
	std::fseek(file, 0, SEEK_END);
	// Get the file length
	ysize size = std::ftell(file);
	// Go to the beginning of the file
	std::rewind(file);
	// Create the content buffer
	m_Buffer = new char[size + 1];
	// Read the whole file
	std::fread(m_Buffer, size, 1, file);
	// NULL terminate
	m_Buffer[size] = '\0';

	// Go throught the content and add line descriptions
	for (ysize offs = 0; offs < size;) {
		// Get the length of the line
		ysize llen = line_len(m_Buffer + offs);
		// Add it and the pointer to the line descriptors
		m_Lines.push_back({ m_Buffer + offs, llen ? llen - 1 : 0 });
		// Advance to the next line
		offs += llen;
	}

	// Close the file
	std::fclose(file);
}

file_hnd::file_hnd(ystr const& path)
	: file_hnd(path.c_str()) {
}

file_hnd::~file_hnd() {
	delete[] m_Buffer;
}

bool file_hnd::good() const {
	return m_Buffer != nullptr;
}

ystr const& file_hnd::path() const {
	return m_Path;
}

const char* file_hnd::buffer() const {
	return m_Buffer;
}

file_hnd::line_info const& file_hnd::line(ysize index) const {
	// Make sure the index is in bounds
	assert(index < m_Lines.size() && "file_hnd::line() index out of bounds!");
	return m_Lines[index];
}

ysize file_hnd::line_count() const {
	return m_Lines.size();
}

file_hnd::line_it file_hnd::lines_begin() const {
	return m_Lines.begin();
}

file_hnd::line_it file_hnd::lines_end() const {
	return m_Lines.end();
}
