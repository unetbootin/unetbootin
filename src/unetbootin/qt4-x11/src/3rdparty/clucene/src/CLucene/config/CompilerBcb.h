/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#if !defined(_lucene_COMPILER_BCB)
#define _lucene_COMPILER_BCB

// It is internal CLucene header - DO NOT include it directly

#include "CLucene/config/define_std.h"
#undef _CL_HAVE_STRTOLL
#undef _CL_HAVE_WCSTOLL

#define _LUCENE_PRAGMA_ONCE
#define _LUCENE_PRAGMA_WARNINGS //tell lucene to display warnings using pragmas instead of #warning
#define LUCENE_DISABLE_HASHING
#define LUCENE_STATIC_CONSTANT(type, assignment) enum { assignment }

#undef LUCENE_ENABLE_MEMLEAKTRACKING //it has been reported that this causes problems

#define fileSize filelength
#define fileSeek lseek
#define fileTell tell
#define fileStat stat
#define fileHandleStat fstat

#define O_RANDOM 0

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

//required type
typedef int intptr_t;

#define _CL_ILONG(x) x ## L
#define _ILONGLONG(x) x ## i64


#elif !defined(_lucene_COMPILER_BCB2)
#define _lucene_COMPILER_BCB2
    //second inclusion

   #define _open open
   #define _timeb timeb
   #define _ftime ::ftime
   #define _rename rename
   
   #define _realpath(rel,abs) _fullpath(abs,rel,CL_MAX_PATH)
   
#elif !defined(_lucene_COMPILER_BCB3)
#define _lucene_COMPILER_BCB3
  //third inclusion

#endif
