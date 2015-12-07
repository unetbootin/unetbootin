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

#include <qcryptographichash.h>

#include "../../3rdparty/md5/md5.h"
#include "../../3rdparty/md5/md5.cpp"
#include "../../3rdparty/md4/md4.h"
#include "../../3rdparty/md4/md4.cpp"
#include "../../3rdparty/sha1/sha1.cpp"


QT_BEGIN_NAMESPACE

class QCryptographicHashPrivate
{
public:
    QCryptographicHash::Algorithm method;
    union {
        MD5Context md5Context;
        md4_context md4Context;
        Sha1State sha1Context;
    };
    QByteArray result;
};

/*!
  \class QCryptographicHash

  \brief The QCryptographicHash class provides a way to generate cryptographic hashes.

  \since 4.3

  \ingroup tools
  \reentrant

  QCryptographicHash can be used to generate cryptographic hashes of binary or text data.
  
  Currently Md4, Md5, and Sha1 are supported.
*/

/*!
  \enum QCryptographicHash::Algorithm

  \value Md4 Generate an Md4 hash sum
  \value Md5 Generate an Md5 hash sum
  \value Sha1 Generate an Sha1 hash sum
*/

/*!
  Constructs an object that can be used to create a cryptographic hash from data using \a method.
*/
QCryptographicHash::QCryptographicHash(Algorithm method)
    : d(new QCryptographicHashPrivate)
{
    d->method = method;
    reset();
}

/*!
  Destroys the object.
*/
QCryptographicHash::~QCryptographicHash()
{
    delete d;
}

/*!
  Resets the object.
*/
void QCryptographicHash::reset()
{
    switch (d->method) {
    case Md4:
        md4_init(&d->md4Context);
        break;
    case Md5:
        MD5Init(&d->md5Context);
        break;
    case Sha1:
        sha1InitState(&d->sha1Context);
        break;
    }
    d->result.clear();
}

/*!
    Adds the first \a length chars of \a data to the cryptographic
    hash.
*/
void QCryptographicHash::addData(const char *data, int length)
{
    switch (d->method) {
    case Md4:
        md4_update(&d->md4Context, (const unsigned char *)data, length);
        break;
    case Md5:
        MD5Update(&d->md5Context, (const unsigned char *)data, length);
        break;
    case Sha1:
        sha1Update(&d->sha1Context, (const unsigned char *)data, length);
        break;
    }    
    d->result.clear();
}

/*!
  /overload
*/
void QCryptographicHash::addData(const QByteArray &data)
{
    addData(data.constData(), data.length());
}

/*!
  Returns the final hash value.
*/
QByteArray QCryptographicHash::result() const
{
    if (!d->result.isEmpty()) 
        return d->result;
    
    switch (d->method) {
    case Md4:
        d->result.resize(MD4_RESULTLEN);
        md4_final(&d->md4Context, (unsigned char *)d->result.data());
        break;
    case Md5:
        d->result.resize(16);
        MD5Final(&d->md5Context, (unsigned char *)d->result.data());
        break;
    case Sha1:
        d->result.resize(20);
        sha1FinalizeState(&d->sha1Context);
        sha1ToHash(&d->sha1Context, (unsigned char *)d->result.data());
    }
    return d->result;
}

/*!
  Returns the hash of \a data using \a method.
*/
QByteArray QCryptographicHash::hash(const QByteArray &data, Algorithm method)
{
    QCryptographicHash hash(method);
    hash.addData(data);
    return hash.result();
}

QT_END_NAMESPACE
