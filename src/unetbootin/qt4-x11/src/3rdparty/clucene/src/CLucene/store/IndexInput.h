/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#ifndef _lucene_store_IndexInput_
#define _lucene_store_IndexInput_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include <QtCore/QString>

#include "CLucene/util/bufferedstream.h"
#include "IndexOutput.h"

CL_NS_DEF(store)

   /** Abstract base class for input from a file in a {@link Directory}.  A
   * random-access input stream.  Used for all Lucene index input operations.
   * @see Directory
   * @see IndexOutput
   */
	class IndexInput: LUCENE_BASE {
	private:
		void skipChars( const int32_t count);
	protected:
		IndexInput();
		IndexInput(const IndexInput& clone);
	public:
	    virtual ~IndexInput(){}
		virtual IndexInput* clone() const =0;

		DEFINE_MUTEX(THIS_LOCK)

		/** Reads and returns a single byte.
		* @see IndexOutput#writeByte(byte)
		*/
		virtual uint8_t readByte() =0;
		
		/** Reads a specified number of bytes into an array at the specified offset.
		* @param b the array to read bytes into
		* @param offset the offset in the array to start storing bytes
		* @param len the number of bytes to read
		* @see IndexOutput#writeBytes(byte[],int32_t)
		*/
		virtual void readBytes(uint8_t* b, const int32_t len) =0;
		
		/** Reads four bytes and returns an int.
		* @see IndexOutput#writeInt(int32_t)
		*/
		int32_t readInt();
		
		/** Reads an int stored in variable-length format.  Reads between one and
		* five bytes.  Smaller values take fewer bytes.  Negative numbers are not
		* supported.
		* @see IndexOutput#writeVInt(int32_t)
		*/
		virtual int32_t readVInt();
		
		/** Reads eight bytes and returns a long.
		* @see IndexOutput#writeLong(long)
		*/
		int64_t readLong();
		
		/** Reads a long stored in variable-length format.  Reads between one and
		* nine bytes.  Smaller values take fewer bytes.  Negative numbers are not
		* supported. */
		int64_t readVLong();
		
		/** Reads a string.
		* @see IndexOutput#writeString(String)
		* maxLength is the amount read into the buffer, the whole string is still read from the stream
		* returns the amount read
		*/
		int32_t readString(TCHAR* buffer, const int32_t maxlength);
		
		/** Reads a string.
		* @see IndexOutput#writeString(String)
		* If unique is true (default) the string will be duplicated.
		* If false and the length is zero, LUCENE_BLANK_STRING is returned 
		*/
		TCHAR* readString(const bool unique=true);
		
		
		/** Reads UTF-8 encoded characters into an array.
		* @param buffer the array to read characters into
		* @param start the offset in the array to start storing characters
		* @param length the number of characters to read
		* @see IndexOutput#writeChars(String,int32_t,int32_t)
		*/
		void readChars( TCHAR* buffer, const int32_t start, const int32_t len);
		
		/** Closes the stream to futher operations. */
		virtual void close() =0;
		
		/** Returns the current position in this file, where the next read will
		* occur.
		* @see #seek(long)
		*/
		virtual int64_t getFilePointer() const =0;
		
		/** Sets current position in this file, where the next read will occur.
		* @see #getFilePointer()
		*/
		virtual void seek(const int64_t pos) =0;
		
		/** The number of bytes in the file. */
		virtual int64_t length() = 0;
		
		virtual QString getDirectoryType() const = 0;
	};
	
   /** Abstract base class for input from a file in a {@link Directory}.  A
   * random-access input stream.  Used for all Lucene index input operations.
   * @see Directory
   * @see IndexOutput
   */
	class BufferedIndexInput: public IndexInput{
	private:
		uint8_t* buffer; //array of bytes
		void refill();
	protected:
		int32_t bufferSize;				//size of the buffer
		int64_t bufferStart;			  // position in file of buffer
		int32_t bufferLength;			  // end of valid l_byte_ts
		int32_t bufferPosition;		  // next uint8_t to read

      /** Returns a clone of this stream.
      *
      * <p>Clones of a stream access the same data, and are positioned at the same
      * point as the stream they were cloned from.
      *
      * <p>Expert: Subclasses must ensure that clones may be positioned at
      * different points in the input from each other and from the stream they
      * were cloned from.
      */
		BufferedIndexInput(const BufferedIndexInput& clone);
		BufferedIndexInput(int32_t bufferSize = CL_NS(store)::BufferedIndexOutput::BUFFER_SIZE);
	public:
		
		virtual ~BufferedIndexInput();
		virtual IndexInput* clone() const = 0;
		void close();
		inline uint8_t readByte(){
			if (bufferPosition >= bufferLength)
				refill();

			return buffer[bufferPosition++];
		}
		void readBytes(uint8_t* b, const int32_t len);
		int64_t getFilePointer() const;
		void seek(const int64_t pos);

	protected:
      /** Expert: implements buffer refill.  Reads bytes from the current position
      * in the input.
      * @param b the array to read bytes into
      * @param offset the offset in the array to start storing bytes
      * @param length the number of bytes to read
      */
		virtual void readInternal(uint8_t* b, const int32_t len) = 0;

      /** Expert: implements seek.  Sets current position in this file, where the
      * next {@link #readInternal(byte[],int32_t,int32_t)} will occur.
      * @see #readInternal(byte[],int32_t,int32_t)
      */
		virtual void seekInternal(const int64_t pos) = 0;
	};
	
	/**
	* JStream InputStream which reads from an IndexInput. This class is 
	* used by the FieldReader to create binary fields. You can then use 
	* a GZipInputStream to read compressed data or any of the other 
	* JStream stream types.
	*
	*/
	class IndexInputStream: public jstreams::BufferedInputStream<char>{
		IndexInput* input;
	public:
		IndexInputStream(IndexInput* input);
		~IndexInputStream();
	    int32_t fillBuffer(char* start, int32_t space);
	};
CL_NS_END
#endif
