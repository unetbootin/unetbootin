/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "Misc.h"

#ifdef _CL_TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if defined(_CL_HAVE_SYS_TIME_H)
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#ifdef _CL_HAVE_SYS_TIMEB_H
# include <sys/timeb.h>
#endif

CL_NS_DEF(util)

size_t Misc::ahashCode(const char* str){
	// Compute the hash code using a local variable to be reentrant.
	size_t hashCode = 0;
	while ( *str != 0 )
		hashCode = hashCode * 31 + *str++;
	return hashCode;
}
size_t Misc::ahashCode(const char* str, size_t len){
	// Compute the hash code using a local variable to be reentrant.
	size_t hashCode = 0;
	for (size_t i = 0; i<len; i++)
		hashCode = hashCode * 31 + *str++;
	return hashCode;
}
#ifdef _UCS2
size_t Misc::whashCode(const wchar_t* str){
	// Compute the hash code using a local variable to be reentrant.
	size_t hashCode = 0;
	while ( *str != 0 )
		hashCode = hashCode * 31 + *str++;
	return hashCode;
}
size_t Misc::whashCode(const wchar_t* str, size_t len){
	// Compute the hash code using a local variable to be reentrant.
	size_t hashCode = 0;
	for (size_t i = 0; i<len; i++)
		hashCode = hashCode * 31 + *str++;
	return hashCode;
}

//static
char* Misc::_wideToChar(const wchar_t* s CL_FILELINEPARAM){
   size_t len = _tcslen(s);
   char* msg=_CL_NEWARRAY(char,len+1);
   _cpywideToChar( s,msg,len+1 );
   return msg;
}
wchar_t* Misc::_charToWide(const char* s CL_FILELINEPARAM){
   size_t len = strlen(s);
   wchar_t* msg = _CL_NEWARRAY(wchar_t,len+1);
   _cpycharToWide(s,msg,len+1);
   return msg;
}

void Misc::_cpywideToChar(const wchar_t* s, char* d, size_t len){
    size_t sLen = wcslen(s);
    for ( uint32_t i=0;i<len&&i<sLen+1;i++ )
        d[i] = LUCENE_OOR_CHAR(s[i]);
}
void Misc::_cpycharToWide(const char* s, wchar_t* d, size_t len){
    size_t sLen = strlen(s);
    for ( uint32_t i=0;i<len&&i<sLen+1;i++ )
      d[i] = s[i];
}
#endif

//static
uint64_t Misc::currentTimeMillis() {
#if defined(_CLCOMPILER_MSVC) || defined(__MINGW32__) || defined(__BORLANDC__)
    struct _timeb tstruct;
    _ftime(&tstruct);

    return (((uint64_t) tstruct.time) * 1000) + tstruct.millitm;
#else
    struct timeval tstruct;
    if (gettimeofday(&tstruct, NULL) < 0) {
    _CLTHROWA(CL_ERR_Runtime,"Error in gettimeofday call.");
    }

    return (((uint64_t) tstruct.tv_sec) * 1000) + tstruct.tv_usec / 1000;
#endif
}
/*
//static
const TCHAR* Misc::replace_all( const TCHAR* val, const TCHAR* srch, const TCHAR* repl )
{
int32_t cnt = 0;
size_t repLen = _tcslen(repl);
size_t srchLen = _tcslen(srch);
size_t srcLen = _tcslen(val);

const TCHAR* pos = val;
while( (pos = _tcsstr(pos+1, srch)) != NULL ) {
    ++cnt;
}

size_t lenNew = (srcLen - (srchLen * cnt)) + (repLen * cnt);
TCHAR* ret = _CL_NEWARRAY(TCHAR,lenNew+1);
ret[lenNew] = 0;
if ( cnt == 0 ){
    _tcscpy(ret,val);
    return ret;
}

TCHAR* cur = ret; //position of return buffer
const TCHAR* lst = val; //position of value buffer
pos = val; //searched position of value buffer
while( (pos = _tcsstr(pos+1,srch)) != NULL ) {
    _tcsncpy(cur,lst,pos-lst); //copy till current
    cur += (pos-lst);
    lst = pos; //move val position

    _tcscpy( cur,repl); //copy replace
    cur += repLen; //move return buffer position
    lst += srchLen; //move last value buffer position
}
_tcscpy(cur, lst ); //copy rest of buffer

return ret;
}
*/
//static
bool Misc::dir_Exists(const char* path){
	if ( !path || !*path )
		return false;
	struct fileStat buf;
	int32_t ret = fileStat(path,&buf);
	return ( ret == 0);
}

int64_t Misc::file_Size(const char* path){
	struct fileStat buf;
	if ( fileStat(path,&buf) == 0 )
		return buf.st_size;
	else
		return -1;
}

//static
/* DSR:CL_BUG: (See comment for join method in Misc.h): */
TCHAR* Misc::join ( const TCHAR* a, const TCHAR* b, const TCHAR* c, const TCHAR* d,const TCHAR* e,const TCHAR* f ) {
#define LEN(x) (x == NULL ? 0 : _tcslen(x))
const size_t totalLen =
    LEN(a) + LEN(b) + LEN(c) + LEN(d) + LEN(e) + LEN(f)
    + sizeof(TCHAR); /* Space for terminator. */

TCHAR* buf = _CL_NEWARRAY(TCHAR,totalLen);
buf[0]=0;
if ( a != NULL) _tcscat(buf,a);
if ( b != NULL) _tcscat(buf,b);
if ( c != NULL) _tcscat(buf,c);
if ( d != NULL) _tcscat(buf,d);
if ( e != NULL) _tcscat(buf,e);
if ( f != NULL) _tcscat(buf,f);
return buf;
}

char* Misc::ajoin ( const char* a, const char* b, const char* c, const char* d,const char* e,const char* f ) {
#define aLEN(x) (x == NULL ? 0 : strlen(x))
const size_t totalLen =
    aLEN(a) + aLEN(b) + aLEN(c) + aLEN(d) + aLEN(e) + aLEN(f)
    + sizeof(char); /* Space for terminator. */

char* buf = _CL_NEWARRAY(char,totalLen);
buf[0]=0;
if ( a != NULL) strcat(buf,a);
if ( b != NULL) strcat(buf,b);
if ( c != NULL) strcat(buf,c);
if ( d != NULL) strcat(buf,d);
if ( e != NULL) strcat(buf,e);
if ( f != NULL) strcat(buf,f);
return buf;
}

//static
bool Misc::priv_isDotDir( const TCHAR* name )
{
if( name[0] == '\0' ) {
    return (false);
}
if( name[0] == '.' && name[1] == '\0' ) {
    return (true);
}
if( name[1] == '\0' ) {
    return (false);
}
if( name[0] == '.' && name[1] == '.' && name[2] == '\0' ) {
    return (true);
}

return (false);
}

//internal static function shared for clucene
char* Misc::segmentname( const char* segment, const char* ext, const int32_t x ){
//Func -  Returns an allocated buffer in which it creates a filename by 
//       concatenating segment with ext and x
//Pre    ext != NULL and holds the extension
//       x contains a number
//Post - A buffer has been instantiated an when x = -1 buffer contains the concatenation of 
//       segment and ext otherwise buffer contains the contentation of segment, ext and x

	CND_PRECONDITION(ext != NULL, "ext is NULL");

	//Allocate a new buffer to hold the concatenated filename
	char* buf = _CL_NEWARRAY(char,CL_MAX_PATH);
	//Concatenate segment and ext and store result in buf
	if ( x==-1 )
		_snprintf(buf,CL_MAX_PATH,"%s%s", segment,ext );
	else
		_snprintf(buf,CL_MAX_PATH,"%s%s%d", segment,ext,x );
	return buf;
}
void Misc::segmentname(char* buffer,int32_t bufferLen, const char* Segment, const char* ext, const int32_t x){
//Func - Static Method
//       Creates a filename in buffer by concatenating Segment with ext and x
//Pre  - buffer  != NULL
//       Segment != NULL and holds the name of the segment
//       ext     != NULL and holds the extension
//       x contains a number
//Post - When x = -1 buffer contains the concatenation of Segment and ext otherwise
//       buffer contains the contentation of Segment, ext and x

	CND_PRECONDITION(buffer  != NULL, "buffer is NULL");
	CND_PRECONDITION(Segment != NULL, "Segment is NULL");
	CND_PRECONDITION(ext     != NULL, "ext is NULL");

	if ( x== -1 )
		_snprintf(buffer,bufferLen,"%s%s", Segment,ext );
	else
		_snprintf(buffer,bufferLen,"%s%s%d", Segment,ext,x );
}


//static 
int32_t Misc::stringDifference(const TCHAR* s1, const int32_t len1, const TCHAR* s2, const int32_t len2) {
	int32_t len = len1 < len2 ? len1 : len2;
	for (int32_t i = 0; i < len; i++)
		if ( s1[i] != s2[i])
			return i;
	return len;
}

CL_NS_END
