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

#ifndef Patternist_QObjectNodeModel_H
#define Patternist_QObjectNodeModel_H

#include "qnamepool_p.h"
#include "qitem_p.h"
#include "qdynamiccontext_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QObject;

namespace QPatternist
{
    class PropertyToAtomicValue;
    /**
     * @short Delegates QtCore's QObject into Patternist's QAbstractXmlNodeModel.
     * known as pre/post numbering.
     *
     * QObjectNodeModel sets the toggle on QXmlNodeModelIndex to @c true, if it
     * represents a property of the QObject. That is, if the QXmlNodeModelIndex is
     * an attribute.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class QObjectNodeModel : public QAbstractXmlNodeModel
    {
    public:
        typedef QExplicitlySharedDataPointer<QObjectNodeModel> Ptr;
        using QAbstractXmlNodeModel::createIndex;
        QObjectNodeModel(const NamePool::Ptr &np);

        /**
         * A QObject tree doesn't have a natural base URI.  For that
         * reason we return QCoreApplication::applicationFilePath().
         */
        virtual QUrl baseUri(const QXmlNodeModelIndex &ni) const;

        /**
         * @short Returns the same as baseURI().
         */
        virtual QUrl documentUri(const QXmlNodeModelIndex &ni) const;

        virtual QXmlNodeModelIndex::NodeKind kind(const QXmlNodeModelIndex &ni) const;
        virtual QXmlNodeModelIndex::DocumentOrder compareOrder(const QXmlNodeModelIndex &ni1, const QXmlNodeModelIndex &ni2) const;
        virtual QXmlNodeModelIndex root(const QXmlNodeModelIndex &n) const;
        virtual QXmlNodeModelIndex parent(const QXmlNodeModelIndex &ni) const;
        virtual QXmlNodeModelIndex::Iterator::Ptr iterate(const QXmlNodeModelIndex &ni, QXmlNodeModelIndex::Axis axis) const;
        virtual QXmlName name(const QXmlNodeModelIndex &ni) const;
        virtual QVector<QXmlName> namespaceBindings(const QXmlNodeModelIndex &n) const;

        /**
         * @short We don't have any namespaces, so we do nothing.
         */
        virtual void sendNamespaces(const QXmlNodeModelIndex &n,
                                    const QExplicitlySharedDataPointer<QAbstractXmlReceiver> &receiver) const;
        virtual QString stringValue(const QXmlNodeModelIndex &n) const;
        virtual QVariant typedValue(const QXmlNodeModelIndex &n) const;
        virtual Item::Iterator::Ptr sequencedTypedValue(const QXmlNodeModelIndex &n) const;
        virtual ItemType::Ptr type(const QXmlNodeModelIndex &ni) const;
        virtual QXmlNodeModelIndex id(const QString &ni) const;
        virtual QXmlNodeModelIndex idref(const QString &ni) const;

        inline QXmlNodeModelIndex mapToItem(QObject *const,
                                            const DynamicContext::Ptr &context) const;

        inline QAbstractXmlForwardIterator<QObject *>::Ptr mapToSequence(const QObject *const,
                                                                      const DynamicContext::Ptr &context) const;

    private:
        friend class QObjectPropertyToAttributeIterator;
        enum
        {
            /**
             * @short if QXmlNodeModelIndex::additionalData() has this flag set, then
             * the QXmlNodeModelIndex is an attribute of the QObject element, and the
             * remaining bits forms an offset to the QObject's property
             * that this QXmlNodeModelIndex corresponds to.
             *
             * The highest bit set in int32.
             */
            IsAttribute = 1 << 31
        };

        static inline QObject *asQObject(const QXmlNodeModelIndex n);
        static inline bool isProperty(const QXmlNodeModelIndex n);
        static inline QMetaProperty toMetaProperty(const QXmlNodeModelIndex n);
        /**
         * Returns the ancestors of @p n. Does therefore not include
         * @p n.
         */
        inline QXmlNodeModelIndex::List ancestors(const QXmlNodeModelIndex n) const;
        const NamePool::Ptr m_namePool;
        const QUrl m_baseURI;
        inline Item propertyValue(const QXmlNodeModelIndex &n) const;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
