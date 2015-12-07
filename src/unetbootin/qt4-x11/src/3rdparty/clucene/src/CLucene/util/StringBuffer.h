/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_util_StringBuffer_
#define _lucene_util_StringBuffer_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif


CL_NS_DEF(util)
  class StringBuffer:LUCENE_BASE{
  public:
   ///Constructor. Allocates a buffer with the default length.
   StringBuffer();
   ///Constructor. Allocates a buffer of length initSize + 1
   StringBuffer(const int32_t initSize);
   ///Constructor. Creates an instance of Stringbuffer containing a copy of 
   ///the string value
   StringBuffer(const TCHAR* value);
   ///Constructs a StringBuffer using another buffer. The StringBuffer can
   ///the be used to easily manipulate the buffer.
   StringBuffer(TCHAR* buf,int32_t maxlen, const bool consumeBuffer);
   ///Destructor
   ~StringBuffer();
   ///Clears the Stringbuffer and resets it to it default empty state
   void clear();
   
   ///Appends a single character 
   void appendChar(const TCHAR chr);
   ///Appends a copy of the string value 
   void append(const TCHAR* value);
   ///Appends a copy of the string value
   void append(const TCHAR* value, size_t appendedLength);
   ///Appends an integer (after conversion to a character string)
   void appendInt(const int32_t value);
   ///Appends a qreal (after conversion to a character string)
   void appendFloat(const qreal value, const int32_t digits);
   ///Puts a copy of the string value in front of the current string in the StringBuffer
   void prepend(const TCHAR* value);
   ///Puts a copy of the string value in front of the current string in the StringBuffer
   void prepend(const TCHAR* value, size_t prependedLength);
   
   ///Contains the length of string in the StringBuffer
   ///Public so that analyzers can edit the length directly
   int32_t len;
   ///Returns the length of the string in the StringBuffer
   int32_t length() const;
   ///Returns a copy of the current string in the StringBuffer
   TCHAR* toString(); 
   ///Returns a null terminated reference to the StringBuffer's text
   TCHAR* getBuffer(); 


   ///reserve a minimum amount of data for the buffer. 
   ///no change made if the buffer is already longer than length
   void reserve(const int32_t length);
  private:
   ///A buffer that contains strings
   TCHAR* buffer;
   ///The length of the buffer
   int32_t bufferLength;
   bool bufferOwner;
   
   ///Has the buffer grown to a minimum length of minLength or bigger
   void growBuffer(const int32_t minLength);
   ///Has the buffer grown to a minimum length of minLength or bigger and shifts the
   ///current string in buffer by skippingNInitialChars forward
   void growBuffer(const int32_t minLength, const int32_t skippingNInitialChars);

  };
CL_NS_END
#endif
