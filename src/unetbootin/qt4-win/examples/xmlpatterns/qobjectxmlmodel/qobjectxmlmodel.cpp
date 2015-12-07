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

#include <QVector>
#include <QtDebug>

#include <QCoreApplication>
#include <QMetaProperty>
#include <QXmlQuery>
#include <QXmlResultItems>

#include "qobjectxmlmodel.h"

QT_BEGIN_NAMESPACE

/*
<metaObjects>
    <metaObject className="QObject"/>
    <metaObject className="QWidget" superClass="QObject">
    </metaObject>
    ...
</metaObjects>
<QObject objectName="MyWidget" property1="..." property2="..."> <!-- This is root() -->
    <QObject objectName="MyFOO" property1="..."/>
    ....
</QObject>
*/

QMetaProperty QObjectXmlModel::toMetaProperty(const QXmlNodeModelIndex &n)
{
    const int propertyOffset = n.additionalData() & (~QObjectProperty);
    const QObject *const qo = asQObject(n);
    return qo->metaObject()->property(propertyOffset);
}

QObjectXmlModel::QObjectXmlModel(QObject *const object,
                                 const QXmlNamePool &np) : QSimpleXmlNodeModel(np)
                                                         , m_baseURI(QUrl::fromLocalFile(QCoreApplication::applicationFilePath()))
                                                         , m_root(object)
                                                         , m_allMetaObjects(allMetaObjects())
{
    Q_ASSERT(m_baseURI.isValid());
}

QXmlNodeModelIndex QObjectXmlModel::qObjectSibling(const int pos,
                                                   const QXmlNodeModelIndex &ni) const
{
    Q_ASSERT(pos == 1 || pos == -1);
    Q_ASSERT(asQObject(ni));

    const QObject *parent = asQObject(ni)->parent();
    if(parent)
    {
        const QList<QObject *> &children = parent->children();
        const int siblingPos = children.indexOf(asQObject(ni)) + pos;

        if(siblingPos >= 0 && siblingPos < children.count())
            return createIndex(children.at(siblingPos));
        else
            return QXmlNodeModelIndex();
    }
    else
        return QXmlNodeModelIndex();
}

QObjectXmlModel::QObjectNodeType QObjectXmlModel::toNodeType(const QXmlNodeModelIndex &ni)
{
    return QObjectNodeType(ni.additionalData() & (15 << 26));
}

QObjectXmlModel::AllMetaObjects QObjectXmlModel::allMetaObjects() const
{
    /* Actually, we could call this in our constructor and store this
     * as a member to avoid it being called each time. Afterall, it's
     * only pointers to existing structures. */

    /* It's easier to do this with QXmlQuery than with C++. */
    QXmlQuery query(namePool());
    query.bindVariable("root", root());
    query.setQuery("declare variable $root external;"
                   "$root/descendant-or-self::QObject");
    Q_ASSERT(query.isValid());

    QXmlResultItems result;
    query.evaluateTo(&result);
    QXmlItem i(result.next());

    AllMetaObjects objects;

    while(!i.isNull())
    {
        const QMetaObject *moo = asQObject(i.toNodeModelIndex())->metaObject();

        while(moo)
        {
            if(!objects.contains(moo))
                objects.append(moo);

            moo = moo->superClass();
        }

        i = result.next();
    }

    Q_ASSERT(!objects.contains(0));
    return objects;
}

QXmlNodeModelIndex QObjectXmlModel::metaObjectSibling(const int pos,
                                                      const QXmlNodeModelIndex &ni) const
{
    Q_ASSERT(pos == 1 || pos == -1);
    Q_ASSERT(!ni.isNull());

    const int indexOf = m_allMetaObjects.indexOf(static_cast<const QMetaObject *>(ni.internalPointer())) + pos;

    if(indexOf >= 0 && indexOf < m_allMetaObjects.count())
        return createIndex(const_cast<QMetaObject *>(m_allMetaObjects.at(indexOf)), MetaObject);
    else
        return QXmlNodeModelIndex();
}

QXmlNodeModelIndex QObjectXmlModel::nextFromSimpleAxis(SimpleAxis axis, const QXmlNodeModelIndex &ni) const
{
    switch(toNodeType(ni))
    {
        case MetaObjects:
        {
            switch(axis)
            {
                case Parent:
                    return QXmlNodeModelIndex();
                case PreviousSibling:
                    return QXmlNodeModelIndex();
                case NextSibling:
                    return root();
                case FirstChild:
                {
                    /* Since we have at least m_root, we're guaranteed to have
                     * at least one meta object. */
                    return createIndex(const_cast<QMetaObject *>(m_allMetaObjects.first()), MetaObject);
                }
            }
            Q_ASSERT(false);
        }
        case MetaObject:
        {
            switch(axis)
            {
                case FirstChild:
                    return QXmlNodeModelIndex();
                case Parent:
                    return createIndex(qint64(0), MetaObjects);
                case PreviousSibling:
                    return metaObjectSibling(-1, ni);
                case NextSibling:
                    return metaObjectSibling(1, ni);
            }
        }
        case IsQObject:
        {
            switch(axis)
            {
                case Parent:
                    return createIndex(asQObject(ni)->parent());
                case FirstChild:
                {
                    if(asQObject(ni)->children().isEmpty())
                        return QXmlNodeModelIndex();
                    else
                        return createIndex(asQObject(ni)->children().first());
                }
                case PreviousSibling:
                {
                    if(asQObject(ni) == m_root)
                        return createIndex(qint64(0), MetaObjects);
                    else
                        return qObjectSibling(-1, ni);
                }
                case NextSibling:
                    return qObjectSibling(1, ni);
            }
            Q_ASSERT(false);
        }
        case MetaObjectClassName:
        /* Fallthrough. */
        case MetaObjectSuperClass:
        {
            Q_ASSERT(axis == Parent);
            return createIndex(asQObject(ni), MetaObject);
        }
        case QObjectClassName:
        /* Fallthrough. */
        case QObjectProperty:
        {
            Q_ASSERT(axis == Parent);
            return createIndex(asQObject(ni));
        }
    }

    Q_ASSERT(false);
    return QXmlNodeModelIndex();
}

QVector<QXmlNodeModelIndex> QObjectXmlModel::attributes(const QXmlNodeModelIndex &ni) const
{
     QVector<QXmlNodeModelIndex> result;

     QObject *const object = asQObject(ni);

     switch(toNodeType(ni))
     {
        case IsQObject:
        {
            const QMetaObject *const metaObject = object->metaObject();
            const int count = metaObject->propertyCount();

            result.append(createIndex(object, QObjectClassName));

            for(int i = 0; i < count; ++i)
                result.append(createIndex(object, QObjectProperty | i));
            return result;
        }
        case MetaObject:
        {
            result.append(createIndex(object, MetaObjectClassName));
            result.append(createIndex(object, MetaObjectSuperClass));
            return result;
        }
        default:
            return QVector<QXmlNodeModelIndex>();
     }
}

QObject *QObjectXmlModel::asQObject(const QXmlNodeModelIndex &n)
{
    return static_cast<QObject *>(n.internalPointer());
}

bool QObjectXmlModel::isProperty(const QXmlNodeModelIndex n)
{
    return n.additionalData() & QObjectProperty;
}

QUrl QObjectXmlModel::documentUri(const QXmlNodeModelIndex &) const
{
    return m_baseURI;
}

QXmlNodeModelIndex::NodeKind QObjectXmlModel::kind(const QXmlNodeModelIndex &ni) const
{
    switch(toNodeType(ni))
    {
        case IsQObject:
        /* Fallthrough. */
        case MetaObject:
        /* Fallthrough. */
        case MetaObjects:
            return QXmlNodeModelIndex::Element;
        case QObjectProperty:
        /* Fallthrough. */
        case MetaObjectClassName:
        /* Fallthrough. */
        case MetaObjectSuperClass:
        /* Fallthrough. */
        case QObjectClassName:
            return QXmlNodeModelIndex::Attribute;
    }

    Q_ASSERT(false);
    return QXmlNodeModelIndex::Element;
}

QXmlNodeModelIndex::DocumentOrder QObjectXmlModel::compareOrder(const QXmlNodeModelIndex &, const QXmlNodeModelIndex &) const
{
    return QXmlNodeModelIndex::Follows; // TODO
}

QXmlNodeModelIndex QObjectXmlModel::root() const
{
    return createIndex(m_root);
}

QXmlNodeModelIndex QObjectXmlModel::root(const QXmlNodeModelIndex &n) const
{
    QObject *p = asQObject(n);
    Q_ASSERT(p);

    do
    {
        QObject *const candidate = p->parent();
        if(candidate)
            p = candidate;
        else
            break;
    }
    while(true);

    return createIndex(p);
}

QXmlNodeModelIndex QObjectXmlModel::parent(const QXmlNodeModelIndex &ni) const
{
    QObject *const p = asQObject(ni)->parent();

    if(p)
        return createIndex(p);
    else
        return QXmlNodeModelIndex();
}

QXmlNodeModelIndex::List QObjectXmlModel::ancestors(const QXmlNodeModelIndex n) const
{
    /* We simply throw all of them into a QList and return an iterator over it. */
    const QObject *p = asQObject(n);
    Q_ASSERT(p);

    QXmlNodeModelIndex::List result;
    do
    {
        QObject *const candidate = p->parent();
        if(candidate)
        {
            result.append(createIndex(candidate, 0));
            p = candidate;
        }
        else
            break;
    }
    while(true);

    return result;
}

QXmlName QObjectXmlModel::name(const QXmlNodeModelIndex &ni) const
{
    switch(toNodeType(ni))
    {
        case IsQObject:
            return QXmlName(namePool(), QLatin1String("QObject"));
        case MetaObject:
            return QXmlName(namePool(), QLatin1String("metaObject"));
        case QObjectClassName:
        /* Fallthrough. */
        case MetaObjectClassName:
            return QXmlName(namePool(), QLatin1String("className"));
        case QObjectProperty:
            return QXmlName(namePool(), toMetaProperty(ni).name());
        case MetaObjects:
            return QXmlName(namePool(), QLatin1String("metaObjects"));
        case MetaObjectSuperClass:
            return QXmlName(namePool(), QLatin1String("superClass"));
    }

    Q_ASSERT(false);
    return QXmlName();
}

QVariant QObjectXmlModel::typedValue(const QXmlNodeModelIndex &n) const
{
    switch(toNodeType(n))
    {
        case QObjectProperty:
        {
           const QVariant &candidate = toMetaProperty(n).read(asQObject(n));
           if(isTypeSupported(candidate.type()))
               return candidate;
           else
               return QVariant();
        }
        case MetaObjectClassName:
            return QVariant(static_cast<QMetaObject *>(n.internalPointer())->className());
        case MetaObjectSuperClass:
        {
            const QMetaObject *const superClass = static_cast<QMetaObject *>(n.internalPointer())->superClass();
            if(superClass)
                return QVariant(superClass->className());
            else
                return QVariant();
        }
        case QObjectClassName:
            return QVariant(asQObject(n)->metaObject()->className());
        default:
            return QVariant();
    }
}

/*!
 Returns \c true if QVariants of type \a type can be used
 in QtXmlPatterns, otherwise \c false.
 */
bool QObjectXmlModel::isTypeSupported(QVariant::Type type)
{
    /* See data/qatomicvalue.cpp too. */
    switch(type)
    {
        /* Fallthrough all these. */
        case QVariant::Char:
        case QVariant::String:
        case QVariant::Url:
        case QVariant::Bool:
        case QVariant::ByteArray:
        case QVariant::Int:
        case QVariant::LongLong:
        case QVariant::ULongLong:
        case QVariant::Date:
        case QVariant::DateTime:
        case QVariant::Time:
        case QVariant::Double:
            return true;
        default:
            return false;
    }
}

QT_END_NAMESPACE
