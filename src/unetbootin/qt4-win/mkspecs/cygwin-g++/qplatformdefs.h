#ifndef QPLATFORMDEFS_H
#define QPLATFORMDEFS_H

// Get Qt defines/settings

#include "qglobal.h"

// Set any POSIX/XOPEN defines at the top of this file to turn on specific APIs

// 1) need to reset default environment if _BSD_SOURCE is defined
// 2) need to specify POSIX thread interfaces explicitly in glibc 2.0
// 3) it seems older glibc need this to include the X/Open stuff
#ifndef _GNU_SOURCE
#  define _GNU_SOURCE
#endif

#include <unistd.h>


// We are hot - unistd.h should have turned on the specific APIs we requested


#include <pthread.h>
#include <dirent.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <signal.h>
#include <dlfcn.h>

#include <sys/types.h>
#include <sys/ioctl.h>
// Cygwin does not provide <sys/ipc.h> and <sys/shm.h> because it
// doesn't support SysV IPC or shared memory. See for example:
// 	http://afni.nimh.nih.gov/afni/afniboard/messages/1725.html
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
//#include <windows.h>
#include <netinet/in.h>


#define QT_STATBUF		struct stat
#define QT_STATBUF4TSTAT	struct stat
#define QT_STAT			::stat
#define QT_FSTAT		::fstat
#define QT_LSTAT		::lstat
#define QT_OPEN                 ::open
#define QT_STAT_REG		S_IFREG
#define QT_STAT_DIR		S_IFDIR
#define QT_STAT_MASK		S_IFMT
#define QT_STAT_LNK		S_IFLNK
#define QT_SOCKET_CONNECT	::connect
#define QT_SOCKET_BIND		::bind
#define QT_FILENO		fileno
#define QT_OPEN			::open
#define QT_CLOSE		::close
#define QT_TRUNCATE		::truncate
#define QT_FTRUNCATE		::ftruncate
#define QT_LSEEK		::lseek
#define QT_READ			::read
#define QT_WRITE		::write
#define QT_ACCESS		::access
#define QT_GETCWD		::getcwd
#define QT_CHDIR		::chdir
#define QT_MKDIR		::mkdir
#define QT_RMDIR		::rmdir
#define QT_OPEN_RDONLY		O_RDONLY
#define QT_OPEN_WRONLY		O_WRONLY
#define QT_OPEN_RDWR		O_RDWR
#define QT_OPEN_CREAT		O_CREAT
#define QT_OPEN_TRUNC		O_TRUNC
#define QT_OPEN_APPEND		O_APPEND
#define QT_FOPEN                ::fopen
#define QT_FOPEN                ::fopen
#define QT_FSEEK                ::fseek
#define QT_FTELL                ::ftell
#define QT_FGETPOS              ::fgetpos
#define QT_FSETPOS              ::fsetpos
#define QT_FPOS_T               fpos_t
#define QT_OFF_T                long

#define QT_SIGNAL_RETTYPE	void
#define QT_SIGNAL_ARGS		int
#define QT_SIGNAL_IGNORE	SIG_IGN

#define QT_SOCKLEN_T		socklen_t

#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE >= 500)
#define QT_SNPRINTF		::snprintf
#define QT_VSNPRINTF		::vsnprintf
#endif


#endif // QPLATFORMDEFS_H
