/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "StringIntern.h"
CL_NS_DEF(util)

	__wcsintrntype::iterator wblank;
	bool blanksinitd=false;
	__wcsintrntype CLStringIntern::stringPool(true);
	__strintrntype CLStringIntern::stringaPool(true);
	DEFINE_MUTEX(CLStringIntern::THIS_LOCK)

    void CLStringIntern::shutdown(){
    #ifdef _DEBUG
		SCOPED_LOCK_MUTEX(THIS_LOCK)
        if ( stringaPool.size() > 0 ){
            printf("ERROR: stringaPool still contains intern'd strings (refcounts):\n");
            __strintrntype::iterator itr = stringaPool.begin();
            while ( itr != stringaPool.end() ){
                printf(" %s (%d)\n",(itr->first), (itr->second));
                ++itr;
            }
        }
        
        if ( stringPool.size() > 0 ){
            printf("ERROR: stringPool still contains intern'd strings (refcounts):\n");
            __wcsintrntype::iterator itr = stringPool.begin();
            while ( itr != stringPool.end() ){
                _tprintf(_T(" %s (%d)\n"),(itr->first), (itr->second));
                ++itr;
            }
        }
    #endif
    }

	const TCHAR* CLStringIntern::intern(const TCHAR* str CL_FILELINEPARAM){
		if ( str == NULL )
			return NULL;
		if ( str[0] == 0 )
			return LUCENE_BLANK_STRING;

		SCOPED_LOCK_MUTEX(THIS_LOCK)

		__wcsintrntype::iterator itr = stringPool.find(str);
		if ( itr==stringPool.end() ){
#ifdef _UCS2
			TCHAR* ret = lucenewcsdup(str CL_FILELINEREF);
#else
			TCHAR* ret = lucenestrdup(str CL_FILELINEREF);
#endif
			stringPool[ret]= 1;
			return ret;
		}else{
			(itr->second)++;
			return itr->first;
		}
	}

	bool CLStringIntern::unintern(const TCHAR* str){
		if ( str == NULL )
			return false;
		if ( str[0] == 0 )
			return false;

		SCOPED_LOCK_MUTEX(THIS_LOCK)

		__wcsintrntype::iterator itr = stringPool.find(str);
		if ( itr != stringPool.end() ){
			if ( (itr->second) == 1 ){
				stringPool.removeitr(itr);
				return true;
			}else
				(itr->second)--;
		}
		return false;
	}
	
	const char* CLStringIntern::internA(const char* str CL_FILELINEPARAM){
		if ( str == NULL )
			return NULL;
		if ( str[0] == 0 )
			return _LUCENE_BLANK_ASTRING;

		SCOPED_LOCK_MUTEX(THIS_LOCK)

		__strintrntype::iterator itr = stringaPool.find(str);
		if ( itr==stringaPool.end() ){
			char* ret = lucenestrdup(str CL_FILELINE);
			stringaPool[ret] = 1;
			return ret;
		}else{
			(itr->second)++;
			return itr->first;
		}
	}
	
	bool CLStringIntern::uninternA(const char* str){
		if ( str == NULL )
			return false;
		if ( str[0] == 0 )
			return false;

		SCOPED_LOCK_MUTEX(THIS_LOCK)

		__strintrntype::iterator itr = stringaPool.find(str);
		if ( itr!=stringaPool.end() ){
			if ( (itr->second) == 1 ){
				stringaPool.removeitr(itr);
				return true;
			}else
				(itr->second)--;
		}
		return false;
	}

	/* removed because of multi-threading problems...
	__wcsintrntype::iterator CLStringIntern::internitr(const TCHAR* str CL_FILELINEPARAM){
		if ( str[0] == 0 ){
			if ( !blanksinitd ){
				CLStringIntern::stringPool.put(LUCENE_BLANK_STRING,1);
				wblank=stringPool.find(str); 
				blanksinitd=true;
			}
			return wblank;
		}
		__wcsintrntype::iterator itr = stringPool.find(str);
		if (itr==stringPool.end()){
#ifdef _UCS2
			TCHAR* ret = lucenewcsdup(str CL_FILELINEREF);
#else
			TCHAR* ret = lucenestrdup(str CL_FILELINEREF);
#endif
			stringPool.put(ret,1);
			return stringPool.find(str);
		}else{
			(itr->second)++;
			return itr;
		}
	}
	bool CLStringIntern::uninternitr(__wcsintrntype::iterator itr){
		if ( itr!=stringPool.end() ){
			if ( itr==wblank )
				return false;	
			if ( (itr->second) == 1 ){
				stringPool.removeitr(itr);
				return true;
			}else
				(itr->second)--;
		}
		return false;
	}
*/

CL_NS_END
