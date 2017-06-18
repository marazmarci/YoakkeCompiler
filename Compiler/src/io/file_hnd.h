/**
 * file_hnd.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  This file contains the main file handle structure along with the exception
 *  that it can throw.
 */

#pragma once

#include "../common.h"

/**
 * This exception is thrown when the file handle could not open the source
 * file from the path it was provided.
 */
struct no_file_exception {
	const ystr Path;

	no_file_exception(ystr const& path)
		: Path(path) {
	}
};

/**
 * This is the actual file handle that contains all the file contents and 
 * line info.
 */
struct file_hnd {
public:
	/**
	 * Line iterator for iterating over pointer-length pairs.
	 */
	using line_it = yvec<ypair<const char*, ysize>>::const_iterator;

public:
	/**
	 * Creates a file handle from the given file path.
	 * @param path The file path as a C string.
	 * @throw no_file_exception if the file could not be opened.
	 */
	file_hnd(const char* path);
	
	/**
	 * Creates a file handle from the given file path.
	 * @param path The file path as a C++ string.
	 * @throw no_file_exception if the file could not be opened.
	 */
	file_hnd(ystr const& path);

	/**
	 * Deallocates the buffer with the file contents.
	 */
	~file_hnd();

public:
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
	ypair<const char*, ysize> const& line(ysize index) const;

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
	ystr							m_Path;		// Original file path
	char*							m_Buffer;	// Content buffer
	yvec<ypair<const char*, ysize>>	m_Lines;	// Line pointer-length pair
};
