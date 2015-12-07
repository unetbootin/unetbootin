/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#include "q3membuf_p.h"

QT_BEGIN_NAMESPACE

// *******************************************************************
// QMembuf declaration and implementation
// *******************************************************************

/*  \internal
    This class implements an efficient buffering of data that is often used by
    asynchronous IO classes like QSocket, QHttp and QProcess.
*/

Q3Membuf::Q3Membuf() : _size(0), _index(0)
{
}

Q3Membuf::~Q3Membuf()
{
    while (!buf.isEmpty())
        delete buf.takeFirst();
}

/*! \internal
    This function consumes \a nbytes bytes of data from the
    buffer and copies it into \a sink. If \a sink is a 0 pointer
    the data goes into the nirvana.
*/
bool Q3Membuf::consumeBytes(Q_ULONG nbytes, char *sink)
{
    if (nbytes <= 0 || (qint64)nbytes > _size)
        return false;
    _size -= nbytes;
    while (!buf.isEmpty()) {
        QByteArray *a = buf.first();
        if ((int)(_index + nbytes) >= a->size()) {
            // Here we skip the whole byte array and get the next later
            int len = a->size() - _index;
            if (sink) {
                memcpy(sink, a->constData()+_index, len);
                sink += len;
            }
            nbytes -= len;
            buf.removeFirst();
	    delete a;
            _index = 0;
            if (nbytes == 0)
                break;
        } else {
            // Here we skip only a part of the first byte array
            if (sink)
                memcpy(sink, a->constData()+_index, nbytes);
            _index += nbytes;
            break;
        }
    }
    return true;
}

/*! \internal
    Scans for any occurrence of '\n' in the buffer. If \a store
    is not 0 the text up to the first '\n' (or terminating 0) is
    written to \a store, and a terminating 0 is appended to \a store
    if necessary. Returns true if a '\n' was found; otherwise returns
    false.
*/
bool Q3Membuf::scanNewline(QByteArray *store)
{
    if (_size == 0)
        return false;
    int i = 0; // index into 'store'
    QByteArray *a = 0;
    char *p;
    int n;
    bool retval = false;
    for (int j = 0; j < buf.size(); ++j) {
        a = buf.at(j);
        p = a->data();
        n = a->size();
        if (!j) {
            // first buffer
            p += _index;
            n -= _index;
        }
        if (store) {
            while (n-- > 0) {
                *(store->data()+i) = *p;
                if (++i == (int)store->size())
                    store->resize(store->size() < 256
                                   ? 1024 : store->size()*4);
                if (*p == '\n') {
                    retval = true;
                    goto end;
                }
                p++;
            }
        } else {
            while (n-- > 0) {
                if(*p == '\n')
                    return true;
                p++;
            }
        }
    }
 end:
    if (store)
        store->resize(i);
    return retval;
}

int Q3Membuf::ungetch(int ch)
{
    if (buf.isEmpty() || _index==0) {
        // we need a new QByteArray
        QByteArray *ba = new QByteArray;
        ba->resize(1);
        buf.prepend(ba);
        _size++;
        (*ba)[0] = ch;
    } else {
        // we can reuse a place in the buffer
        QByteArray *ba = buf.first();
        _index--;
        _size++;
        (*ba)[(int)_index] = ch;
    }
    return ch;
}

QT_END_NAMESPACE
