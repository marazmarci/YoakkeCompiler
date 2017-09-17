/**
 * file_hnd.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  This file contains the main file handle structure.
 */

#pragma once

#include "../common.h"

/**
 * This is the actual file handle that contains all the file contents and
 * line info.
 */
struct file_hnd {
public:
	/**
	 * Line information.
	 */
	using line_info = ytup<const char*, ysize>;

	/**
	 * Line iterator for iterating over pointer-length pairs.
	 */
	using line_it = yvec<line_info>::const_iterator;

public:
	// Illegal operations
	file_hnd(file_hnd const&) = delete;
	file_hnd& operator=(file_hnd const&) = delete;

	/**
	 * Creates a file handle from the given file path.
	 * @param path The file path as a C string.
	 */
	file_hnd(const char* path);

	/**
	 * Creates a file handle from the given file path.
	 * @param path The file path as a C++ string.
	 */
	file_hnd(ystr const& path);

	/**
	 * Creates a fake file handle to simulate files used for testing.
	 * @param alias The name of the fake file.
	 * @param src The file contents.
	 */
	file_hnd(ystr const& alias, ystr const& src);

	/**
	 * Deallocates the buffer with the file contents.
	 */
	~file_hnd();

public:
	/**
	 * Checks if the file was successfully read.
	 * @return True, if the file handle is good.
	 */
	bool good() const;

	/**
	 * Gets the path that was provided for the file as source.
	 * @return The source file path.
	 */
	ystr const& path() const;

	/**
	 * Gets the pointer to the beginning of the file contents for reading
	 * or iteration.
	 * @return A pointer to the beginning of the contents.
	 */
	const char* buffer() const;

	/**
	 * Gets a pointer-length pair of the line at the given index.
	 * @param index The index of the line starting from 0.
	 * @return A pair with the pointer to the beginning of the line
	 * and the line length.
	 */
	line_info const& line(ysize index) const;

	/**
	 * Gets the number of lines of the file.
	 * @return The number if lines in the file.
	 */
	ysize line_count() const;

	/**
	 * Returns an iterator for the first line.
	 * @return A beginning iterator.
	 */
	line_it lines_begin() const;

	/**
	 * Returns an iterator just after the lines.
	 * @return An ending iterator
	 */
	line_it lines_end() const;

private:
	/**
	 * Registers the lines based on the source buffer.
	 * Used by constructors.
	 * @param size The length of the buffer.
	 */
	void init_lines(ysize size);

private:
	ystr			m_Path;		// Original file path
	char*			m_Buffer;	// Content buffer
	yvec<line_info>	m_Lines;	// Line pointer-length pair
};
