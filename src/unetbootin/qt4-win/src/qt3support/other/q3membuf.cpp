/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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
