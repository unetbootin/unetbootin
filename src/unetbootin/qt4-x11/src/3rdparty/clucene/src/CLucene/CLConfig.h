/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_Config_
#define _lucene_Config_


////////////////////////////////////////////////////////////////////
//   this settings should be set up in the compiler, 
//   but are put here for reference as to what could be defined
////////////////////////////////////////////////////////////////////
//
//define this if you want debugging code to be enabled
//#define _DEBUG
//
//define this if you want condition debugging to be enabled
#if defined(_DEBUG) && !defined(_CL__CND_DEBUG)
 #define _CL__CND_DEBUG
#endif
//
//define this to print out lots of information about merges, etc
//requires __CL__CND_DEBUG to be defined
//#define _CL_DEBUG_INFO stdout
//
//to disable namespaces define this
//#define DISABLE_NAMESPACE
//
//This is mostly for windows. If you have put the google sparse
//map code in your include path somewhere, then define this
//to use it.
//However, for msvc, there are no significant gains since there
//is already a compatible hashmap available.
//#define _CL_HAVE_GOOGLE_DENSE_HASH_MAP
//
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//   These options can be set depending on the particular needs of
//   Your application
////////////////////////////////////////////////////////////////////
//
//define this to force the build into ascii mode
//#define _ASCII
//
//define this to force the build into ucs2 mode
//#define _UCS2
//
//if a wide character is being converted to a ascii character and it
//cannot fit, this character is used instead. Required.
#define LUCENE_OOR_CHAR(c) ((char)(((unsigned short)c)&0xFF))
//
//define if you would like to force clucene to use the internal
//character functions.
//Tests may display unpredictable behaviour if this is not defined.
#define LUCENE_USE_INTERNAL_CHAR_FUNCTIONS
//
//define this to enable mmap support in the fsdirectory IndexInput
//todo: only available for windows so far...need to add MMapInput.cpp to project
//EXPERIMENTAL
//#define LUCENE_FS_MMAP
//
//LOCK_DIR implementation:
//define this to set an exact directory for the lock dir (not recommended)
//all other methods of getting the temporary directory will be ignored
//#define LUCENE_LOCK_DIR "/tmp"
//
//define this to try and load the lock dir from this specified environment variable
#define LUCENE_LOCK_DIR_ENV_1 "TEMP"
//define this if you want to have look up this environment variable if the first one fails
#define LUCENE_LOCK_DIR_ENV_2 "TMP"
//define this if you want to have a fallback directory, if not defined then 
//the lockdirectory will be the index directory
#define LUCENE_LOCK_DIR_ENV_FALLBACK "/tmp"
//
////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////
//   The following are search query options
//   THe NO_* options can make CLucene faster and/or smaller
//   special queries sometime require longer search times or may 
//   not be required
////////////////////////////////////////////////////////////////////
//
//Define this to remove fuzzy query and sloppy scoring
//#define NO_FUZZY_QUERY
//
//Define to remove wildcard t*m or te?m to match term
//#define NO_WILDCARD_QUERY
//
//Define to remove prefix term query - ter* to match term or terms
//#define NO_PREFIX_QUERY
//
//Define to remove range (exlusive and inclusive)
//#define NO_RANGE_QUERY
//
//This must always be defined. They can be adjusted if required. But
//general Wildcard string would be '*' and Wildcard Char would be '?'
//Both are Required.
#define LUCENE_WILDCARDTERMENUM_WILDCARD_STRING '*'
#define LUCENE_WILDCARDTERMENUM_WILDCARD_CHAR   '?'
//
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//   memory handling configurations
////////////////////////////////////////////////////////////////////
//
//If this is defined, lucene's configurations are changed
//to use less memory, but may run slower.
//todo: i dont think this actualy changes speed much, just memory
#define LUCENE_OPTIMIZE_FOR_MEMORY
//
//define this if you want the pointer tracking to be enabled
//this is a useful tool for memory leak tracking
//The LuceneBase can slow down the code a *lot*
#if defined(_DEBUG)
	#if !defined(LUCENE_DISABLE_MEMTRACKING) && !defined(LUCENE_ENABLE_MEMLEAKTRACKING)
		#define LUCENE_ENABLE_MEMLEAKTRACKING
	#endif
#endif 
//
//enable use of rich file/line tracking. use CL_FILELINE to pass
//to functions like stringDuplicate (or use CL_STRDUP* functions instead) and
//CLStringIntern::x.
#if defined(LUCENE_ENABLE_MEMLEAKTRACKING)
	#define LUCENE_ENABLE_FILELINEINFO
#endif
//
//enable creation of clucene.log file. Logs every
//call to new operator. Must have LUCENE_ENABLE_MEMLEAKTRACKING enabled.
//writes log in this format.
//action,file name,file line,allocation size
//logging can be disabled by setting _lucene_disable_debuglogging to true
#if defined(LUCENE_ENABLE_MEMLEAKTRACKING) && defined(_DEBUG)
//#define LUCENE_ENABLE_CONSTRUCTOR_LOG
#endif
//
//
//enable this if you want to enable reference counting. This is
//not necessary or useful in most cases except when implementing wrappers 
//which have reference counting. If the wrapper wraps a StringReader,
//for example, it should expect that the wrapped StringReader should not
//be deleted. However, when the stringreader is added into a Field,
//the Field usually takes over the stringReader and deletes it on completion.
//If reference counting is enabled, the wrapper can add a reference to any class
//and when _CLDECDELETE is called, the reference is decremented and only deleted
//if the refcount is zero.
#define LUCENE_ENABLE_REFCOUNT


////////////////////////////////////////////////////////////////////
//   These options allow you to remove certain implementations
//   out of clucene so that they can be implemented in the client
//   application
////////////////////////////////////////////////////////////////////
//
//define this to your own setting if you would like to implement your own
//threading locking code. it should have the same sort of functions as
//mutex_default. If not defined, clucene will try and use posix,win32 critical
//sections, or a timer based mutex hack.
//#define _LUCENE_THREADMUTEX CL_NS(util)::mutex_default
//
//define this if you want to implement the _Cnd_OutDebug routine yourself
//you can then easily customise in your own application how to handle debug messages
//#define _CND_DEBUG_DONTIMPLEMENT_OUTDEBUG
//
//define this if you want to implement your own namespace macros
//#define _LUCENE_DONTIMPLEMENT_NS_MACROS
//
//define this if you do not want clucene to include any standard libraries.
//this could be useful if you want to use alternate libraries
//#define LUCENE_DISABLE_INCLUDES
//
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
//   These options will be changed depending on your compiler/platform
//   but can also be changed here if required
////////////////////////////////////////////////////////////////////
//
//define this if multi-threading support is not required
//if not defined, multi-thread locking will
//occur (and its related processing overhead)
//note: it is recommended to disable multithreading if you do not need it
//there is a lot of overhead that can be avoided.
//#define _CL_DISABLE_MULTITHREADING
//
//if you want to define your own default file encoding. specify it
//here - normally defined in the platform specific headers
//#define PLATFORM_DEFAULT_READER_ENCODING CL_NS(util)::FileReader::ENCODING_ASCII
//
//disable hash implementations (if available)
//#define LUCENE_DISABLE_HASHING
////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////
//   These options should not be changed. But you can experiment with
//   them to optimize performance
////////////////////////////////////////////////////////////////////
//
//some defaults, wouldn't usually need to be changed
//Buffer size for input/output streams. Required.
#define LUCENE_STREAM_BUFFER_SIZE 1024
//
// DSR:2004.08.19:
// Formerly, StringBuffer used 1024 as the default size of its internal buffer.
// However, StringBuffer is used primarily for token- and term-oriented
// processing, e.g. in StandardTokenizer.  I've calculated that the average
// token (as produced by StandardTokenizer) in all .txt files distributed in
// the Project Gutenberg CD Image (August 2003 release) has only 6 characters.
// Although most languages are likely to have a longer average word length than
// English due to the popularity of "non-atomized" conjugation and declension
// mechanisms, 1024 is still vastly excessive.
// I made two changes intended to deliver better overall performance:
//   a) Switched to a default StringBuffer character capacity of 32.  Though 32
//      is longer than the average token, the high cost of realloc makes a
//      slightly liberal default size optimal.  I chose the default size of 32
//      after fairly extensive experimentation on the Gutenberg e-texts.  The
//      results are summarized in the following table:
//      ------------------------------------------------------------------------
//      LUCENE_DEFAULT_TOKEN_BUFFER_SIZE value | % faster than default size 1024
//      ------------------------------------------------------------------------
//                                           8 : 4%
//                                          16 : 7%
//                                          32 : 6%
//                                          64 : 3%
//      A default size of 32 is actually slightly slower than 16, but I was
//      experimenting on English text; I expect that 32 will maintain decent
//      performance in languages such as German, and in technical documents
//      with long tokens.
//
//   b) To offset the switch to a smaller default buffer size, I implemented a
//      more aggressive growth strategy.  A StringBuffer now [at least] doubles
//      the size of its internal buffer every time it needs to grow, rather
//      than [at least] increasing by LUCENE_DEFAULT_TOKEN_BUFFER_SIZE no
//      matter how many times it has already grown.
//Required.
#define LUCENE_DEFAULT_TOKEN_BUFFER_SIZE 32
//todo: should implement a similar strategy in analysis/token
//
//Expert: The fraction of {@link TermDocs} entries stored in skip tables,
//used to accellerate {@link TermDocs#skipTo(int)}.  Larger values result in
//smaller indices, greater acceleration, but fewer accelerable cases, while
//smaller values result in bigger indices, less acceleration and more
//accelerable cases. More detailed experiments would be useful here. */
#define LUCENE_DEFAULT_TERMDOCS_SKIP_INTERVAL 16
//
//Size of TermScore cache. Required.
#define LUCENE_SCORE_CACHE_SIZE 32
//
//analysis options
//maximum length that the CharTokenizer uses. Required.
//By adjusting this value, you can greatly improve the performance of searching
//and especially indexing. Default is 255, but smaller numbers will decrease
//the amount of memory used as well as increasing the speed.
#define  LUCENE_MAX_WORD_LEN 255
//Maximum length of a token word. 
//Should be the same or more than LUCENE_MAX_WORD_LEN
//if not defined, then no token limit, but may be slower
//if defined will be faster (up to 15% in some cases), but will use more memory
#ifndef LUCENE_OPTIMIZE_FOR_MEMORY
 #define LUCENE_TOKEN_WORD_LENGTH LUCENE_MAX_WORD_LEN
#endif
//
//maximum field length. some optimisation can be done if a maximum field
//length is given... The smaller the better
#define LUCENE_MAX_FIELD_LEN 100
//
//The initial value set to BooleanQuery::maxClauseCount. Default is 1024
#define LUCENE_BOOLEANQUERY_MAXCLAUSECOUNT 1024
//
//bvk: 12.3.2005
//==============================================================================
//Previously the way the tokenizer has worked has been changed to optionally
//use a a fixed word length. I have implemented this in the Term class as well.
//It seems that by predefining the text length instead of using new TCHAR[x]
//in the constructor greatly improves the performance by 20-30% for certain
//operations.
//Maximum length of a term text. 
//Should be the same or more than LUCENE_MAX_WORD_LEN
//if not defined, then no term text limit, but may be slower
//if defined will be faster (up to 30% in some cases), but will use more memory
#ifndef LUCENE_OPTIMIZE_FOR_MEMORY
 #define LUCENE_TERM_TEXT_LENGTH LUCENE_MAX_WORD_LEN
#endif
//
//Size of the CharTokenizer buffersize. Required.
#define LUCENE_IO_BUFFER_SIZE 1024
//
//the minimum amount the segment term enum should grow by. Must be at least 1
#define LUCENE_SEGMENTTERMENUM_GROWSIZE 8
//
////////////////////////////////////////////////////////////////////

#endif

