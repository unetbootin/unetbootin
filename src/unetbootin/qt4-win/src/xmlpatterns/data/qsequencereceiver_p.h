/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
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

#ifndef Patternist_SequenceReceiver_H
#define Patternist_SequenceReceiver_H

#include <QSharedData>

#include "qitem_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short A push interface for the XPath Data Model. Similar to SAX's
     * ContentHandler.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class QAbstractXmlReceiver : public QSharedData
    {
    public:
        typedef QExplicitlySharedDataPointer<QAbstractXmlReceiver> Ptr;

        inline QAbstractXmlReceiver()
        {
        }

        virtual ~QAbstractXmlReceiver();

        /**
         * @short Signals the start of an element by name @p name.
         */
        virtual void startElement(const QXmlName name) = 0;

        /**
         * @short Signals the presence of the namespace declaration @p nb.
         *
         * This event is received @c after startElement(), as opposed to
         * SAX, and before any attribute() events.
         */
        virtual void namespaceBinding(const QXmlName &nb) = 0;

        /**
         * @short Signals the end of the current element.
         */
        virtual void endElement() = 0;

        /**
         * @short Signals the presence of an attribute node.
         *
         * This function is guaranteed by the caller to always be
         * called after a call to startElement() or attribute().
         *
         * @param name the name of the attribute. Guaranteed to always be
         * non-null.
         * @param value the value of the attribute. Guaranteed to always be
         * non-null.
         */
        virtual void attribute(const QXmlName name,
                               const QString &value) = 0;

        virtual void processingInstruction(const QXmlName name,
                                           const QString &value) = 0;
        virtual void comment(const QString &value) = 0;

        /**
         * @short Sends an Item to this QAbstractXmlReceiver that may be a QXmlNodeModelIndex or an
         * AtomicValue.
         */
        virtual void item(const Item &item) = 0;

        /**
         * Sends a text node with value @p value. Adjascent text nodes
         * may be sent. There's no restrictions on @p value, beyond that it
         * must be valid XML characters. For instance, @p value may contain
         * only whitespace.
         *
         * @see whitespaceOnly()
         */
        virtual void characters(const QString &value) = 0;

        /**
         * This function may be called instead of characters() if, and only if,
         * @p value consists only of whitespace.
         *
         * The caller gurantees that @p value, is not empty.
         *
         * By whitespace is meant a sequence of characters that are either
         * spaces, tabs, or the two new line characters, in any order. In
         * other words, the whole of Unicode's whitespace category is not
         * considered whitespace.
         *
         * However, there's no guarantee or requirement that whitespaceOnly()
         * is called for text nodes containing whitespace only, characters()
         * may be called just as well. This is why the default implementation
         * for whitespaceOnly() calls characters().
         *
         * @see characters()
         */
        virtual void whitespaceOnly(const QStringRef &value);

        /**
         * Start of a document node.
         */
        virtual void startDocument() = 0;

        /**
         * End of a document node.
         */
        virtual void endDocument() = 0;

    protected:
        /**
         * Treats @p outputItem as an node and calls the appropriate function,
         * such as attribute() or comment(), depending on its QXmlNodeModelIndex::NodeKind.
         *
         * This a helper function sub-classes can use to multi-plex Nodes received
         * via item().
         *
         * @param outputItem must be a QXmlNodeModelIndex.
         */
        void sendAsNode(const Item &outputItem);

    private:
        /**
         * Call sendAsNode() for each child of @p node. As consistent with the
         * XPath Data Model, this does not include attribute nodes.
         */
        template<const QXmlNodeModelIndex::Axis axis>
        inline void sendFromAxis(const QXmlNodeModelIndex &node);
        Q_DISABLE_COPY(QAbstractXmlReceiver)
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
