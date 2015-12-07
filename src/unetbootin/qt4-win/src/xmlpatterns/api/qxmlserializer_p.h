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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef QXMLSERIALIZER_P_H
#define QXMLSERIALIZER_P_H

#include <QtCore/QIODevice>
#include <QtCore/QStack>
#include <QtCore/QTextCodec>
#include <QtXmlPatterns/QXmlQuery>
#include <QtXmlPatterns/QXmlNamePool>
#include <QtXmlPatterns/QXmlSerializer>

#include "qnamepool_p.h"
#include "qabstractxmlreceiver_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QXmlSerializerPrivate : public QAbstractXmlReceiverPrivate
{
public:
    QXmlSerializerPrivate(const QXmlQuery &q,
                          QIODevice *outputDevice);

    QStack<QPair<QXmlName, bool> >      hasClosedElement;
    bool                                isPreviousAtomic;
    QXmlSerializer::State               state;
    const QPatternist::NamePool::Ptr    np;

    /**
     * This member worries me a bit. We never use it but nevertheless
     * it is pushed and pops linear to startElement() and endElement().
     * An optimization would be to at least merge it with hasClosedElement,
     * but even better to push it on demand. That is, namespaceBinding()
     * pushes it up to the tree depth first when it is needed.
     */
    QStack<QVector<QXmlName> >          namespaces;

    QIODevice *                         device;
    const QTextCodec *                  codec;
    QTextCodec::ConverterState          converterState;
    /**
     * Name cache. Since encoding QStrings are rather expensive
     * operations to do, and we on top of that would have to do
     * it each time a name appears, we here map names to their
     * encoded equivalents.
     *
     * This means that when writing out large documents, the serialization
     * of names after a while is reduced to a hash lookup and passing an
     * existing byte array.
     *
     * We use QXmlName::Code as key as opposed to merely QName, because the
     * prefix is of significance.
     */
    QHash<QXmlName::Code, QByteArray>   nameCache;
    const QXmlQuery                     query;

    inline void write(const char c);

private:
    enum Constants
    {
        EstimatedTreeDepth = 10,

        /**
         * We use a high count to avoid rehashing. We can afford it since we
         * only allocate one hash for this.
         */
        EstimatedNameCount = 60
    };
};

void QXmlSerializerPrivate::write(const char c)
{
    device->putChar(c);
}
QT_END_NAMESPACE
QT_END_HEADER

#endif
