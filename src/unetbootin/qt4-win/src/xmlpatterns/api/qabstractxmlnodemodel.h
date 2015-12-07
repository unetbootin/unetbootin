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

#ifndef QABSTRACTXMLNODEMODEL_H
#define QABSTRACTXMLNODEMODEL_H

#include <QtXmlPatterns/QXmlName>
#include <QtCore/QSharedData>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

QT_MODULE(XmlPatterns)

/* This file contains the classes QXmlNodeModelIndex, QAbstractXmlNodeModel,
 * QXmlItem and QPatternist::NodeIndexStorage. */

class QAbstractXmlNodeModel;
class QAbstractXmlNodeModelPrivate;
class QAbstractXmlReceiver;
class QUrl;
class QXmlName;
class QXmlNodeModelIndex;
template<typename T> class QAbstractXmlForwardIterator;
template<typename T> class QVector;

/* The members in the namespace QPatternist are internal, not part of the public API, and
 * unsupported. Using them leads to undefined behavior. */
namespace QPatternist
{
    class DynamicContext;
    class Item;
    class ItemType;
    template<typename TResult, typename TSource, typename TMapper, typename Context> class ItemMappingIterator;
    template<typename TResult, typename TSource, typename TMapper> class SequenceMappingIterator;
    typedef QExplicitlySharedDataPointer<ItemType> ItemTypePtr;
    typedef QExplicitlySharedDataPointer<QAbstractXmlForwardIterator<Item> > ItemIteratorPtr;
    typedef QVector<QXmlName> QXmlNameVector;

    class NodeIndexStorage
    {
    public:
        typedef qint64 Data;

        union
        {
            void *ptr;
            Data data;
        };

        Data additionalData;
        const QAbstractXmlNodeModel *model;

        /* Implementation is in qabstractxmlnodemodel.cpp. */
        inline bool operator!=(const NodeIndexStorage &other) const;
    };
}

class Q_XMLPATTERNS_EXPORT QXmlNodeModelIndex
{
    enum Constants
    {
        ForwardAxis         = 8192,
        ReverseAxis         = 16384
    };

public:
    inline QXmlNodeModelIndex()
    {
        reset();
    }

    inline QXmlNodeModelIndex(const QXmlNodeModelIndex &other) : m_storage(other.m_storage)
    {
    }

    bool operator==(const QXmlNodeModelIndex &other) const;
    bool operator!=(const QXmlNodeModelIndex &other) const;

    typedef QAbstractXmlForwardIterator<QXmlNodeModelIndex> Iterator;
    typedef QList<QXmlNodeModelIndex> List;

    enum NodeKind
    {
        Attribute               = 1,
        Comment                 = 2,
        Document                = 4,
        Element                 = 8,
        Namespace               = 16,
        ProcessingInstruction   = 32,
        Text                    = 64
    };

    enum DocumentOrder
    {
        Precedes = -1,
        Is       = 0,
        Follows  = 1
    };

    enum Axis
    {
        AxisChild               = 1 | ForwardAxis,
        AxisDescendant          = 2 | ForwardAxis,
        AxisAttribute           = 4 | ForwardAxis,
        AxisSelf                = 8 | ForwardAxis,
        AxisDescendantOrSelf    = 16 | ForwardAxis,
        AxisFollowingSibling    = 32 | ForwardAxis,
        AxisNamespace           = 64 | ForwardAxis,
        AxisFollowing           = 128 | ReverseAxis,
        AxisParent              = 256 | ReverseAxis,
        AxisAncestor            = 512 | ReverseAxis,
        AxisPrecedingSibling    = 1024 | ReverseAxis,
        AxisPreceding           = 2048 | ReverseAxis,
        AxisAncestorOrSelf      = 4096 | ReverseAxis
    };

    inline qint64 data() const
    {
        return m_storage.data;
    }

    inline void *internalPointer() const
    {
        return m_storage.ptr;
    }

    inline const QAbstractXmlNodeModel *model() const
    {
        return m_storage.model;
    }

    inline qint64 additionalData() const
    {
        return m_storage.additionalData;
    }

    inline bool isNull() const
    {
        return !m_storage.model;
    }

    /* The members below are internal, not part of the public API, and
     * unsupported. Using them leads to undefined behavior. */

    inline QXmlName name() const;
    inline QXmlNodeModelIndex root() const;
    inline QExplicitlySharedDataPointer<QAbstractXmlForwardIterator<QXmlNodeModelIndex> > iterate(const Axis axis) const;
    inline QExplicitlySharedDataPointer<QAbstractXmlForwardIterator<QPatternist::Item> > sequencedTypedValue() const;
    inline QUrl documentUri() const;
    inline QUrl baseUri() const;
    inline NodeKind kind() const;
    inline bool isDeepEqual(const QXmlNodeModelIndex &other) const;
    inline DocumentOrder compareOrder(const QXmlNodeModelIndex &other) const;
    inline void sendNamespaces(QAbstractXmlReceiver *const receiver) const;
    inline QVector<QXmlName> namespaceBindings() const;
    inline QXmlName::NamespaceCode namespaceForPrefix(const QXmlName::PrefixCode prefix) const;
    inline QString stringValue() const;
    inline QPatternist::ItemTypePtr type() const;
    inline bool is(const QXmlNodeModelIndex &other) const;

    inline void reset()
    {
        m_storage.data = 0;
        m_storage.additionalData = 0;
        m_storage.model = 0;
    }

private:
    static inline QXmlNodeModelIndex create(const qint64 d,
                                            const QAbstractXmlNodeModel *const nm)
    {
        QXmlNodeModelIndex n;
        n.m_storage.data = d;
        n.m_storage.model = nm;
        n.m_storage.additionalData = 0;
        return n;
    }

    static inline QXmlNodeModelIndex create(const qint64 data,
                                            const QAbstractXmlNodeModel *const nm,
                                            const qint64 addData)
    {
        QXmlNodeModelIndex n;
        n.m_storage.data = data;
        n.m_storage.model = nm;
        n.m_storage.additionalData = addData;
        return n;
    }

    inline QXmlNodeModelIndex(const QPatternist::NodeIndexStorage &storage) : m_storage(storage)
    {
    }

    friend class QAbstractXmlNodeModel;
    friend class QPatternist::Item;
    friend class QXmlItem;
    inline operator int() const; // Disable

    QPatternist::NodeIndexStorage m_storage;
};

Q_XMLPATTERNS_EXPORT uint qHash(const QXmlNodeModelIndex &index);

inline bool qIsForwardIteratorEnd(const QXmlNodeModelIndex &item)
{
    return item.isNull();
}

class Q_XMLPATTERNS_EXPORT QAbstractXmlNodeModel : public QSharedData
{
public:
    enum SimpleAxis
    {
        Parent,
        FirstChild,
        PreviousSibling,
        NextSibling
    };

    typedef QExplicitlySharedDataPointer<QAbstractXmlNodeModel> Ptr;
    typedef QList<Ptr> List;

    QAbstractXmlNodeModel();
    virtual ~QAbstractXmlNodeModel();

    virtual QUrl baseUri(const QXmlNodeModelIndex &ni) const = 0;
    virtual QUrl documentUri(const QXmlNodeModelIndex &ni) const = 0;
    virtual QXmlNodeModelIndex::NodeKind kind(const QXmlNodeModelIndex &ni) const = 0;
    virtual QXmlNodeModelIndex::DocumentOrder compareOrder(const QXmlNodeModelIndex &ni1,
                                                           const QXmlNodeModelIndex &ni2) const = 0;
    virtual QXmlNodeModelIndex root(const QXmlNodeModelIndex &n) const = 0;
    virtual QXmlName name(const QXmlNodeModelIndex &ni) const = 0;
    virtual QString stringValue(const QXmlNodeModelIndex &n) const = 0;
    virtual QVariant typedValue(const QXmlNodeModelIndex &n) const = 0;

    /* The members below are internal, not part of the public API, and
     * unsupported. Using them leads to undefined behavior. */
    virtual QExplicitlySharedDataPointer<QAbstractXmlForwardIterator<QXmlNodeModelIndex> > iterate(const QXmlNodeModelIndex &ni, QXmlNodeModelIndex::Axis axis) const;
    virtual QPatternist::ItemIteratorPtr sequencedTypedValue(const QXmlNodeModelIndex &ni) const;
    virtual QPatternist::ItemTypePtr type(const QXmlNodeModelIndex &ni) const;
    virtual QXmlName::NamespaceCode namespaceForPrefix(const QXmlNodeModelIndex &ni,
                                                       const QXmlName::PrefixCode prefix) const;
    virtual bool isDeepEqual(const QXmlNodeModelIndex &ni1,
                             const QXmlNodeModelIndex &ni2) const;
    virtual void sendNamespaces(const QXmlNodeModelIndex &n,
                                QAbstractXmlReceiver *const receiver) const;
    virtual QVector<QXmlName> namespaceBindings(const QXmlNodeModelIndex &n) const = 0;


    virtual QXmlNodeModelIndex elementById(const QXmlName &NCName) const = 0;
    virtual QVector<QXmlNodeModelIndex> nodesByIdref(const QXmlName &NCName) const = 0;

    enum NodeCopySetting
    {
        InheritNamespaces   = 0x1,
        PreserveNamespaces  = 0x2
    };

    typedef QFlags<NodeCopySetting> NodeCopySettings;
    virtual void copyNodeTo(const QXmlNodeModelIndex &node,
                            QAbstractXmlReceiver *const receiver,
                            const NodeCopySettings &) const;

protected:

    virtual QXmlNodeModelIndex nextFromSimpleAxis(SimpleAxis axis, const QXmlNodeModelIndex &origin) const = 0;
    virtual QVector<QXmlNodeModelIndex> attributes(const QXmlNodeModelIndex &element) const = 0;

    QAbstractXmlNodeModel(QAbstractXmlNodeModelPrivate *d);

    inline QXmlNodeModelIndex createIndex(qint64 data) const
    {
        return QXmlNodeModelIndex::create(data, this);
    }

    inline QXmlNodeModelIndex createIndex(void * pointer,
                                          qint64 additionalData = 0) const
    {
        return QXmlNodeModelIndex::create(qptrdiff(pointer), this, additionalData);
    }

    inline QXmlNodeModelIndex createIndex(qint64 data,
                                          qint64 additionalData) const
    {
        return QXmlNodeModelIndex::create(data, this, additionalData);
    }

    QAbstractXmlNodeModelPrivate *d_ptr;
private:
    friend class QPatternist::ItemMappingIterator<QXmlNodeModelIndex, QXmlNodeModelIndex, const QAbstractXmlNodeModel *, QExplicitlySharedDataPointer<QPatternist::DynamicContext> >;
    friend class QPatternist::SequenceMappingIterator<QXmlNodeModelIndex, QXmlNodeModelIndex, const QAbstractXmlNodeModel *>;

    inline QExplicitlySharedDataPointer<QAbstractXmlForwardIterator<QXmlNodeModelIndex> > mapToSequence(const QXmlNodeModelIndex &ni,
                                                                           const QExplicitlySharedDataPointer<QPatternist::DynamicContext> &) const;

    static inline bool isIgnorableInDeepEqual(const QXmlNodeModelIndex &n);
    Q_DISABLE_COPY(QAbstractXmlNodeModel)
};

Q_DECLARE_TYPEINFO(QXmlNodeModelIndex, Q_MOVABLE_TYPE);

template<typename T> class QAbstractXmlForwardIterator;
class QVariant;
class QXmlItemPrivate;

namespace QPatternist
{
    class AtomicValue;
    class VariableLoader;
    class IteratorBridge;
    class ToQXmlItemMapper;
    class ToItemMapper;
}

class Q_XMLPATTERNS_EXPORT QXmlItem
{
public:
    typedef QAbstractXmlForwardIterator<QXmlItem> Iterator;

    QXmlItem();
    QXmlItem(const QXmlItem &other);
    QXmlItem(const QXmlNodeModelIndex &node);
    QXmlItem(const QVariant &atomicValue);
    ~QXmlItem();
    QXmlItem &operator=(const QXmlItem &other);

    bool isNull() const;
    bool isNode() const;
    bool isAtomicValue() const;

    QVariant toAtomicValue() const;
    QXmlNodeModelIndex toNodeModelIndex() const;

private:
    friend class QPatternist::IteratorBridge;
    friend class QPatternist::VariableLoader;
    friend class QPatternist::ToQXmlItemMapper;
    friend class QPatternist::ToItemMapper;
    friend class QPatternist::Item;

    inline bool internalIsAtomicValue() const;

    inline QXmlItem(const QPatternist::Item &i);

    union
    {
        QPatternist::NodeIndexStorage   m_node;

        /* These two sits at the position of NodeIndexStorage::data.
         * NodeIndexStorage::{additionalData,model} are free. */
        const QPatternist::AtomicValue *m_atomicValue;
        QXmlItemPrivate *               m_ptr; /* Not currently used. */
    };
};

inline bool qIsForwardIteratorEnd(const QXmlItem &item)
{
    return item.isNull();
}

Q_DECLARE_TYPEINFO(QXmlItem, Q_MOVABLE_TYPE);

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QXmlItem); /* This macro must appear after QT_END_NAMESPACE. */

QT_END_HEADER

#endif
