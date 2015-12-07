/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_store_Lock_
#define _lucene_store_Lock_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

CL_NS_DEF(store)
  class LuceneLock: LUCENE_BASE{
  public:
      LUCENE_STATIC_CONSTANT(int64_t, LOCK_POLL_INTERVAL = 1000);

      /** Attempts to obtain exclusive access and immediately return
      *  upon success or failure.
      * @return true iff exclusive access is obtained
      */
      virtual bool obtain() = 0;

      /** Attempts to obtain an exclusive lock within amount
      *  of time given. Currently polls once per second until
      *  lockWaitTimeout is passed.
      * @param lockWaitTimeout length of time to wait in ms
      * @return true if lock was obtained
      * @throws IOException if lock wait times out or obtain() throws an IOException
      */
      bool obtain(int64_t lockWaitTimeout);

      // Release exclusive access.
      virtual void release() = 0;

      /** Returns true if the resource is currently locked.  Note that one must
      * still call {@link #obtain()} before using the resource. */
      virtual bool isLocked() = 0;

      virtual ~LuceneLock()
      {
      }
      
      virtual TCHAR* toString() = 0;
  };


  // Utility class for executing code with exclusive access.
  template<typename T>
  class LuceneLockWith {
  private:
    LuceneLock* lock;
    int64_t lockWaitTimeout;

  protected:
    // Code to execute with exclusive access.
    virtual T doBody() = 0;

  // Constructs an executor that will grab the named lock.
  public:   
    /** Constructs an executor that will grab the named lock.
     *  Defaults lockWaitTimeout to LUCENE_COMMIT_LOCK_TIMEOUT.
     *  @deprecated Kept only to avoid breaking existing code.
     */
    LuceneLockWith(LuceneLock* lock, int64_t lockWaitTimeout) {
      this->lock = lock;
      this->lockWaitTimeout = lockWaitTimeout;
    }
    virtual ~LuceneLockWith(){
	} 

    /** Calls {@link #doBody} while <i>lock</i> is obtained.  Blocks if lock
     * cannot be obtained immediately.  Retries to obtain lock once per second
     * until it is obtained, or until it has tried ten times. Lock is released when
     * {@link #doBody} exits. */
    T runAndReturn() {
        bool locked = false;
        T ret = NULL;
        try {
            locked = lock->obtain(lockWaitTimeout);
            ret = doBody();
        }_CLFINALLY(
            if (locked) 
                lock->release();
        );
		return ret;
    }

	/** @see runAndReturn
     * Same as runAndReturn, except doesn't return any value.
	 * The only difference is that no void values are used
	 */
	void run() {
        bool locked = false;
        try {
            locked = lock->obtain(lockWaitTimeout);
            doBody();
        }_CLFINALLY(
            if (locked) 
                lock->release();
        );
    }
  };

CL_NS_END
#endif
