/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the qmake spec of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPLATFORMDEFS_H
#define QPLATFORMDEFS_H

// Set any POSIX/XOPEN defines at the top of this file to turn on specific APIs
#define _XOPEN_SOURCE 500
#define __EXTENSIONS__

// Get Qt defines/settings

#include "qglobal.h"

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
#include <sys/filio.h>
#include <sys/ipc.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <netinet/in.h>
#ifndef QT_NO_IPV6IFNAME
#include <net/if.h>
#endif

// On 64-bit platforms sockets use socklen_t
#define QT_SOCKLEN_T		socklen_t

// Solaris redefines connect -> __xnet_connect with _XOPEN_SOURCE_EXTENDED
static inline int qt_socket_connect(int s, struct sockaddr *addr, QT_SOCKLEN_T addrlen)
{ return ::connect(s, addr, addrlen); }

#ifdef QT_LARGEFILE_SUPPORT
#define QT_STATBUF              struct stat64
#define QT_STATBUF4TSTAT        struct stat64
#define QT_STAT                 ::stat64
#define QT_FSTAT                ::fstat64
#define QT_LSTAT                ::lstat64
#define QT_OPEN                 ::open64
#define QT_TRUNCATE             ::truncate64
#define QT_FTRUNCATE            ::ftruncate64
#define QT_LSEEK                ::lseek64
#else
#define QT_STATBUF		struct stat
#define QT_STATBUF4TSTAT	struct stat
#define QT_STAT			::stat
#define QT_FSTAT		::fstat
#define QT_LSTAT		::lstat
#define QT_OPEN                 ::open
#define QT_TRUNCATE             ::truncate
#define QT_FTRUNCATE            ::ftruncate
#define QT_LSEEK                ::lseek
#endif

#ifdef QT_LARGEFILE_SUPPORT
#define QT_FOPEN                ::fopen64
#define QT_FSEEK                ::fseeko64
#define QT_FTELL                ::ftello64
#define QT_FGETPOS              ::fgetpos64
#define QT_FSETPOS              ::fsetpos64
#define QT_FPOS_T               fpos64_t
#define QT_OFF_T                off64_t
#else
#define QT_FOPEN                ::fopen
#define QT_FSEEK                ::fseek
#define QT_FTELL                ::ftell
#define QT_FGETPOS              ::fgetpos
#define QT_FSETPOS              ::fsetpos
#define QT_FPOS_T               fpos_t
#define QT_OFF_T                long
#endif

#define QT_STAT_REG		S_IFREG
#define QT_STAT_DIR		S_IFDIR
#define QT_STAT_MASK		S_IFMT
#define QT_STAT_LNK		S_IFLNK
#define QT_SOCKET_CONNECT	qt_socket_connect
#define QT_SOCKET_BIND		::bind
#define QT_FILENO		fileno
#define QT_CLOSE                ::close
#define QT_READ			::read
#define QT_WRITE		::write
#define QT_ACCESS		::access
#define QT_GETCWD		::getcwd
#define QT_CHDIR		::chdir
#define QT_MKDIR		::mkdir
#define QT_RMDIR		::rmdir
#define QT_OPEN_LARGEFILE       O_LARGEFILE
#define QT_OPEN_RDONLY		O_RDONLY
#define QT_OPEN_WRONLY		O_WRONLY
#define QT_OPEN_RDWR		O_RDWR
#define QT_OPEN_CREAT		O_CREAT
#define QT_OPEN_TRUNC		O_TRUNC
#define QT_OPEN_APPEND		O_APPEND

#define QT_SIGNAL_RETTYPE	void
#define QT_SIGNAL_ARGS		int
#define QT_SIGNAL_IGNORE	SIG_IGN

// Only Solaris 7 and better support 64-bit
#define QT_SNPRINTF		::snprintf
#define QT_VSNPRINTF		::vsnprintf

#ifdef connect
#undef connect
#endif

#endif // QPLATFORMDEFS_H
