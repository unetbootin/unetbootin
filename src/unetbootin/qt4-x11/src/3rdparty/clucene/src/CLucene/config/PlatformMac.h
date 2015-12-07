/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
// It is internal CLucene header - DO NOT include it directly

# define PATH_DELIMITER _T("/")
# define PATH_DELIMITERA "/"
# define PATH_DELIMITERC '/'

#  if (__GNUC__ < 3) && !defined( __APPLE_CC__)
// GCC strange "ignore std" mode works better if you pretend everything
// is in the std namespace, for the most part.
#    define LUCENE_NO_STDC_NAMESPACE
#  endif

#undef _T //apple has something else strange here...
