/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
*
* Changes are Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
------------------------------------------------------------------------------*/
#if !defined(_lucene_COMPILER_MSVC)
#define _lucene_COMPILER_MSVC

// It is internal CLucene header - DO NOT include it directly

#include "CLucene/config/define_std.h"

#if (_MSC_VER >= 1300)
//>= 7.0
    #if defined(_BUILD_FOR_QT_)
        # pragma warning(disable: 4100) // disable unreferenced formal parameter
        # pragma warning(disable: 4189) // disable local variable is initialized but not referenced
    #endif
	# pragma warning(disable: 4512) // This would be very annoying
	# pragma warning(disable: 4290) // Ignore exception specification warning
    # pragma warning(disable: 4250) // Ignore 'class1' : inherits 'class2::member' via dominance (e.g. in MultiReader)
	#if (_MSC_VER < 1310)
		#define CL_NS_HASHING(func) std::func //the namespace is different on VC 7.0
	#else
		#define CL_NS_HASHING(func) stdext::func
	#endif
	#define LUCENE_STATIC_CONSTANT_SYNTAX 1

    #if _MSC_FULL_VER >= 140050320
     #define _CL_DEPRECATE_TEXT(_Text) __declspec(deprecated(_Text))
    #else
     #define _CL_DEPRECATE_TEXT(_Text) __declspec(deprecated)
    #endif

#elif (_MSC_VER >= 1200)
//6.0
#ifdef LUCENE_ENABLE_MEMLEAKTRACKING
	#define _CLDELETE_CARRAY(x) if (x!=NULL){CL_NS(debug)::LuceneBase::__cl_voidpremove((void*)x,__FILE__,__LINE__);delete[] __CONST_CAST(TCHAR*,x); x=NULL;}
	#define _CLDELETE_CaARRAY(x) if (x!=NULL){CL_NS(debug)::LuceneBase::__cl_voidpremove((void*)x,__FILE__,__LINE__);delete[] __CONST_CAST(char*,x); x=NULL;}
	#define _CLDELETE_LCARRAY(x) if (x!=NULL){CL_NS(debug)::LuceneBase::__cl_voidpremove((void*)x,__FILE__,__LINE__);delete[] __CONST_CAST(TCHAR*,x);}
	#define _CLDELETE_LCaARRAY(x) if (x!=NULL){CL_NS(debug)::LuceneBase::__cl_voidpremove((void*)x,__FILE__,__LINE__);delete[] __CONST_CAST(char*,x);}
#else
	#define _CLDELETE_CARRAY(x) if (x!=NULL){delete[] __CONST_CAST(TCHAR*,x); x=NULL;}
	#define _CLDELETE_CaARRAY(x) if (x!=NULL){delete[] __CONST_CAST(char*,x); x=NULL;}
	#define _CLDELETE_LCARRAY(x) if (x!=NULL){delete[] __CONST_CAST(TCHAR*,x);}
	#define _CLDELETE_LCaARRAY(x) if (x!=NULL){delete[] __CONST_CAST(char*,x);}
	
#endif
	#define LUCENE_STATIC_CONSTANT_SYNTAX 2

	# pragma warning(disable: 4786) // This would be very annoying
	namespace std{
		# undef min // just in case
		# undef max // just in case
		
		#define min(a,b) (a>b?b:a)
		#define max(a,b) (a>b?a:b)
	}
	
	//only 7.0+ has these function	
	#undef _CL_HAVE_LLTOA
	#undef _CL_HAVE_LLTOAW
	#undef _CL_HAVE_INTPTR_T
	#undef _CL_HAVE_WCSTOLL
	#undef _CL_HAVE_STRTOLL
	#undef _CL_HAVE_HASH_MAP
	#undef _CL_HAVE_HASH_SET

#else
# error "This version of MSVC has not been tested. Please uncomment this line to try anyway. Please send a report to the Clucene's administration if successful"
#endif

#if _MSC_VER >= 1020
 #define _LUCENE_PRAGMA_ONCE
#endif
#define _LUCENE_PRAGMA_WARNINGS //tell lucene to display warnings using pragmas instead of #warning

//if we are compiling using single-threaded libraries, we can disable multi-threading stuff
#if !defined(_MT) && !defined(_CL_DISABLE_MULTITHREADING)
   #define _CL_DISABLE_MULTITHREADING
#endif

//msvc supports large files
#ifdef _LARGE_FILES
# define fileSize _filelengthi64
# define fileSeek _lseeki64
# define fileTell _telli64
# define fileStat _stati64
# define fileHandleStat _fstati64
#else
# define fileSize _filelength
# define fileSeek _lseek
# define fileTell _tell
# define fileStat _stat
# define fileHandleStat _fstat
#endif

//_rename is not defined???
#define _rename rename

#define CL_MAX_PATH 260 //give the windef.h value for this...
#define _realpath(rel,abs) _fullpath(abs,rel,CL_MAX_PATH)

//java long type
typedef __int64 int64_t; 
typedef unsigned __int64 uint64_t;

//java int type
typedef int int32_t;
typedef unsigned int uint32_t;

//java byte type
typedef unsigned char uint8_t;

//floating point type
//we are going to use qreal now
//typedef double float_t;

#define _CL_ILONG(x) x ## L
#define _ILONGLONG(x) x ## i64


#elif !defined(_lucene_COMPILER_MSVC2)
#define _lucene_COMPILER_MSVC2
    //second inclusion


#elif !defined(_lucene_COMPILER_MSVC3)
#define _lucene_COMPILER_MSVC3
  //third inclusion
#endif
