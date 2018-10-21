/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef GARGOYLE_STREAMS_H
#define GARGOYLE_STREAMS_H

#include "common/scummsys.h"
#include "gargoyle/glk_types.h"

namespace Gargoyle {

class GargoyleEngine;
class Window;
class Streams;

struct StreamResult {
	uint32 _readCount;
	uint32 _writeCount;
};

/**
 * Base class for streams
 */
class Stream {
public:
	Streams *_streams;
	Stream *_prev;
	Stream *_next;
	uint32 _rock;
	bool _unicode;
	uint32 _readCount;
	uint32 _writeCount;
	bool _readable, _writable;
public:
	/**
	 * Constructor
	 */
	Stream(Streams *streams, bool readable, bool writable, uint32 rock, bool unicode);

	/**
	 * Destructor
	 */
	virtual ~Stream();

	/**
	 * Get the next stream
	 */
	Stream *getNext(uint32 *rock) const;

	/**
	 * Get the rock value for the stream
	 */
	uint32 getRock() const { return _rock; }

	/**
	 * Fill out the total amount read and/or written
	 */
	void fillResult(StreamResult *result);

	/**
	 * Close and delete the stream
	 */
	void close(StreamResult *result = nullptr);

	/**
	 * Write a character
	 */
	virtual void writeChar(unsigned char ch) = 0;

	/**
	 * Write a unicode character
	 */
	virtual void writeCharUni(uint32 ch) = 0;
};
typedef Stream *strid_t;

/**
 * Implements the stream for writing text to a window
 */
class WindowStream : public Stream {
private:
	Window *_window;
public:
	/**
	 * Constructor
	 */
	WindowStream(Streams *streams, Window *window, uint32 rock = 0, bool unicode = true) :
		Stream(streams, true, false, rock, unicode), _window(window) {}

	/**
	 * Close the stream
	 */
	virtual void close(StreamResult *result = nullptr);

	/**
	 * Write a character
	 */
	virtual void writeChar(unsigned char ch) override;

	/**
	 * Write a unicode character
	 */
	virtual void writeCharUni(uint32 ch) override;
};

/**
 * Implements an in-memory stream
 */
class MemoryStream : public Stream {
private:
	void *_buf;		///< unsigned char* for latin1, glui32* for unicode
	void *_bufptr;
	void *_bufend;
	void *_bufeof;
	size_t _buflen;	///< # of bytes for latin1, # of 4-byte words for unicode
public:
	/**
	 * Constructor
	 */
	MemoryStream(Streams *streams, void *buf, size_t buflen, FileMode mode, uint32 rock = 0, bool unicode = true);

	/**
	 * Write a character
	 */
	virtual void writeChar(unsigned char ch);

	/**
	 * Write a unicode character
	 */
	virtual void writeCharUni(uint32 ch);
};

/**
 * Streams manager
 */
class Streams {
	friend class Stream;
private:
	GargoyleEngine *_engine;
	Stream *_streamList;
	Stream *_currentStream;
private:
	/**
	 * Adds a created stream to the list
	 */
	void addStream(Stream *stream);

	/**
	 * Remove a stream
	 */
	void removeStream(Stream *stream);
public:
	/**
	 * Constructor
	 */
	Streams(GargoyleEngine *engine);

	/**
	 * Destructor
	 */
	~Streams();

	/**
	 * Add a window stream
	 */
	WindowStream *addWindowStream(Window *window);

	/**
	 * Add a memory stream
	 */
	MemoryStream *addMemoryStream(void *buf, size_t buflen, FileMode mode, uint32 rock = 0, bool unicode = true);

	/**
	 * Delete a stream
	 */
	void deleteStream(Stream *stream) {
		delete stream;
	}

	/**
	 * Start an Iteration through streams
	 */
	Stream *getFirst(uint32 *rock);

	/**
	 * Set the current output stream
	 */
	void setCurrent(Stream *stream) {
		assert(stream->_writable);
		_currentStream = stream;
	}

	/**
	 * Gets the current output stream
	 */
	Stream *getCurrent() const { return _currentStream; }
};

} // End of namespace Gargoyle

#endif