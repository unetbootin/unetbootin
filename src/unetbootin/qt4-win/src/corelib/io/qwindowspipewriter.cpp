/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "qwindowspipewriter_p.h"

QT_BEGIN_NAMESPACE
        
#ifndef QT_NO_THREAD

QWindowsPipeWriter::QWindowsPipeWriter(HANDLE pipe, QObject * parent)
    : QThread(parent),
      writePipe(INVALID_HANDLE_VALUE),
      quitNow(false),
      hasWritten(false)
{
#if !defined(Q_OS_WINCE) || (_WIN32_WCE >= 0x600) 
    DuplicateHandle(GetCurrentProcess(), pipe, GetCurrentProcess(),
                         &writePipe, 0, FALSE, DUPLICATE_SAME_ACCESS);
#else
    writePipe = GetCurrentProcess();
#endif
}

QWindowsPipeWriter::~QWindowsPipeWriter()
{
    lock.lock();
    quitNow = true;
    waitCondition.wakeOne();
    lock.unlock();
    if (!wait(100))
        terminate();
#if !defined(Q_OS_WINCE) || (_WIN32_WCE >= 0x600) 
    CloseHandle(writePipe);
#endif
}

bool QWindowsPipeWriter::waitForWrite(int msecs)
{
    QMutexLocker locker(&lock);
    bool hadWritten = hasWritten;
    hasWritten = false;
    if (hadWritten)
        return true;
    if (!waitCondition.wait(&lock, msecs))
        return false;
    hadWritten = hasWritten;
    hasWritten = false;
    return hadWritten;
}

qint64 QWindowsPipeWriter::write(const char *ptr, qint64 maxlen)
{
    if (!isRunning())
        return -1;

    QMutexLocker locker(&lock);
    data.append(QByteArray(ptr, maxlen));
    waitCondition.wakeOne();
    return maxlen;
}

void QWindowsPipeWriter::run()
{
    OVERLAPPED overl = {0, 0, 0, 0, NULL};
    overl.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    forever {
        lock.lock();
        while(data.isEmpty() && (!quitNow)) {
            waitCondition.wakeOne();
            waitCondition.wait(&lock);
        }

        if (quitNow) {
            lock.unlock();
            quitNow = false;
	    break;
        }

        QByteArray copy = data;

        lock.unlock();

        const char *ptrData = copy.data();
        qint64 maxlen = copy.size();
        qint64 totalWritten = 0;
        overl.Offset = 0;
        overl.OffsetHigh = 0;
        while ((!quitNow) && totalWritten < maxlen) {
            DWORD written = 0;
            // Write 2k at a time to prevent flooding the pipe. If you
            // write too much (4k-8k), the pipe can close
            // unexpectedly.
            if (!WriteFile(writePipe, ptrData + totalWritten,
                    qMin<int>(2048, maxlen - totalWritten), &written, &overl)) {
                if (GetLastError() == 0xE8/*NT_STATUS_INVALID_USER_BUFFER*/) {
                    // give the os a rest
                    msleep(100);
                    continue;
                }
#ifndef Q_OS_WINCE
                if (GetLastError() == ERROR_IO_PENDING) {
                  if (!GetOverlappedResult(writePipe, &overl, &written, TRUE)) {
                      CloseHandle(overl.hEvent);
                      return;
                  }
                } else {
                    CloseHandle(overl.hEvent);
                    return;
                }
#else
                return;
#endif
            }
            totalWritten += written;
#if defined QPIPEWRITER_DEBUG
            qDebug("QWindowsPipeWriter::run() wrote %d %d/%d bytes",
			    written, int(totalWritten), int(maxlen));
#endif
            lock.lock();
            data.remove(0, written);
            hasWritten = true;
            lock.unlock();
        }
        emit canWrite();
    }
    CloseHandle(overl.hEvent);
}

#endif //QT_NO_THREAD

QT_END_NAMESPACE
