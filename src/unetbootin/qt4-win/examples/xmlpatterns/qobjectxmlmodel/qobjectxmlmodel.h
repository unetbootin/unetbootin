/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

#ifndef Patternist_QObjectNodeModel_H
#define Patternist_QObjectNodeModel_H

#include <QSimpleXmlNodeModel>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

class QObject;
class PropertyToAtomicValue;

/**
 * @short Delegates QtCore's QObject into Patternist's QAbstractXmlNodeModel.
 * known as pre/post numbering.
 *
 * QObjectXmlModel sets the toggle on QXmlNodeModelIndex to @c true, if it
 * represents a property of the QObject. That is, if the QXmlNodeModelIndex is
 * an attribute.
 *
 * @author Frans Englich <fenglich@trolltech.com>
 */
class QObjectXmlModel : public QSimpleXmlNodeModel
{
public:
    QObjectXmlModel(QObject *const object,
                    const QXmlNamePool &np);

    QXmlNodeModelIndex root() const;

    /**
     * @short Returns the same as baseURI().
     */
    virtual QUrl documentUri(const QXmlNodeModelIndex &ni) const;

    virtual QXmlNodeModelIndex::NodeKind kind(const QXmlNodeModelIndex &ni) const;
    virtual QXmlNodeModelIndex::DocumentOrder compareOrder(const QXmlNodeModelIndex &ni1, const QXmlNodeModelIndex &ni2) const;
    virtual QXmlNodeModelIndex root(const QXmlNodeModelIndex &n) const;
    virtual QXmlNodeModelIndex parent(const QXmlNodeModelIndex &ni) const;
    virtual QXmlName name(const QXmlNodeModelIndex &ni) const;
    virtual QVariant typedValue(const QXmlNodeModelIndex &n) const;
    virtual QXmlNodeModelIndex nextFromSimpleAxis(SimpleAxis, const QXmlNodeModelIndex&) const;
    virtual QVector<QXmlNodeModelIndex> attributes(const QXmlNodeModelIndex&) const;

private:
    /**
     * The highest three bits are used to signify whether the node index
     * is an artificial node.
     */
    enum QObjectNodeType
    {
        IsQObject               = 0,
        /**
         * @short if QXmlNodeModelIndex::additionalData() has this flag set, then
         * the QXmlNodeModelIndex is an attribute of the QObject element, and the
         * remaining bits forms an offset to the QObject's property
         * that this QXmlNodeModelIndex corresponds to.
         */
        QObjectProperty         = 1 << 26,
        MetaObjects             = 2 << 26,
        MetaObject              = 3 << 26,

        /**
         * metaObjects/metaObject/@superClass
         */
        MetaObjectClassName     = 4 << 26,
        /**
         * metaObjects/metaObject/@superClass
         */
        MetaObjectSuperClass    = 5 << 26,
        /**
         * QObject/@className
         */
        QObjectClassName        = 6 << 26
    };

    typedef QVector<const QMetaObject *> AllMetaObjects;
    AllMetaObjects allMetaObjects() const;

    static QObjectNodeType toNodeType(const QXmlNodeModelIndex &ni);

    static bool isTypeSupported(QVariant::Type type);
    static inline QObject *asQObject(const QXmlNodeModelIndex &n);
    static inline bool isProperty(const QXmlNodeModelIndex n);
    static inline QMetaProperty toMetaProperty(const QXmlNodeModelIndex &n);
    /**
     * Returns the ancestors of @p n. Does therefore not include
     * @p n.
     */
    inline QXmlNodeModelIndex::List ancestors(const QXmlNodeModelIndex n) const;
    QXmlNodeModelIndex qObjectSibling(const int pos,
                                      const QXmlNodeModelIndex &ni) const;
    QXmlNodeModelIndex metaObjectSibling(const int pos,
                                         const QXmlNodeModelIndex &ni) const;

    const QUrl              m_baseURI;
    QObject *const          m_root;
    const AllMetaObjects    m_allMetaObjects;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
