/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
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

#ifndef QSOURCELOCATION_H
#define QSOURCELOCATION_H

#include <QtCore/QMetaType>
#include <QtCore/QUrl>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(XmlPatterns)

class QSourceLocationPrivate;

class Q_XMLPATTERNS_EXPORT QSourceLocation
{
public:
    QSourceLocation();
    QSourceLocation(const QSourceLocation &other);
    QSourceLocation(const QUrl &uri, int line = -1, int column = -1);
    ~QSourceLocation();
    QSourceLocation &operator=(const QSourceLocation &other);
    bool operator==(const QSourceLocation &other) const;
    bool operator!=(const QSourceLocation &other) const;

    qint64 column() const;
    void setColumn(qint64 newColumn);

    qint64 line() const;
    void setLine(qint64 newLine);

    QUrl uri() const;
    void setUri(const QUrl &newUri);
    bool isNull() const;

private:
    union
    {
        qint64 m_line;
        QSourceLocationPrivate *m_ptr;
    };
    qint64 m_column;
    QUrl m_uri;
};

Q_XMLPATTERNS_EXPORT uint qHash(const QSourceLocation &location);

#ifndef QT_NO_DEBUG_STREAM
Q_XMLPATTERNS_EXPORT QDebug operator<<(QDebug debug, const QSourceLocation &sourceLocation);
#endif

Q_DECLARE_TYPEINFO(QSourceLocation, Q_MOVABLE_TYPE);

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QSourceLocation); /* This macro must appear after QT_END_NAMESPACE. */

QT_END_HEADER

#endif
