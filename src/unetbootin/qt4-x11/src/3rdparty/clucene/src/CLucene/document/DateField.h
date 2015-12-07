/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_document_DateField_
#define _lucene_document_DateField_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

CL_NS_DEF(document)

//here are some constants used throughout clucene
//make date strings long enough to last a millenium
#define DATEFIELD_DATE_MAX _ILONGLONG(31536000000000) //1000L*365*24*60*60*1000

#define DATEFIELD_DATE_LEN 9 ////Long.toString(DATEFIELD_DATE_MAX, Character.MAX_RADIX).length()

/**
* Provides support for converting dates to strings and vice-versa.
* The strings are structured so that lexicographic sorting orders by date,
* which makes them suitable for use as field values and search terms.
*
* <P>Note that this class saves dates with millisecond granularity,
* which is bad for {@link RangeQuery} and {@link PrefixQuery}, as those
* queries are expanded to a BooleanQuery with a potentially large number
* of terms when searching. Thus you might want to use
* {@link DateTools} instead.
*
* <P>
* Note: dates before 1970 cannot be used, and therefore cannot be
* indexed when using this class. See {@link DateTools} for an
* alternative without such a limitation.
*
* @deprecated If you build a new index, use {@link DateTools} instead. This class is included for use with existing
* indices and will be removed in a future release.
*/
class DateField :LUCENE_BASE {
public:
	~DateField();
	
	/**
	* Converts a millisecond time to a string suitable for indexing.
	* @throws RuntimeException if the time specified in the
	* method argument is negative, that is, before 1970
	*/
	static TCHAR* timeToString(const int64_t time);
	
	/**
	* Converts a millisecond time to a string suitable for indexing.
	* @throws CL_ERR_IllegalArgument if the time specified in the
	* method argument is negative, that is, before 1970
	* @param str must be a character array DATEFIELD_DATE_LEN+1 or longer
	*/
	static void timeToString(const int64_t time, TCHAR* str);
	
	/** Converts a string-encoded date into a millisecond time. */
	static int64_t stringToTime(const TCHAR* s);
};
CL_NS_END
#endif
