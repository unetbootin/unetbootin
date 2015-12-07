/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "Lock.h"

CL_NS_DEF(store)

   bool LuceneLock::obtain(int64_t lockWaitTimeout) {
      bool locked = obtain();
      int maxSleepCount = (int)(lockWaitTimeout / LOCK_POLL_INTERVAL);
      int sleepCount = 0;
      while (!locked) {
         if (sleepCount++ == maxSleepCount) {
            _CLTHROWA(CL_ERR_IO,"Lock obtain timed out");
         }
         _LUCENE_SLEEP(LOCK_POLL_INTERVAL);
         locked = obtain();
      }
      return locked;
   }


CL_NS_END
