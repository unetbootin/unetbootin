/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#include "qdesigner_introspection_p.h"

#include <QtCore/QMetaObject>
#include <QtCore/QMetaEnum>
#include <QtCore/QStringList>
#include <QtCore/QVector>

QT_BEGIN_NAMESPACE

// Qt Implementation
static QStringList byteArrayListToStringList(const QList<QByteArray> &l)
{
    if (l.empty())
        return QStringList();
    QStringList rc;
    const QList<QByteArray>::const_iterator cend = l.constEnd();
    for (QList<QByteArray>::const_iterator it = l.constBegin(); it != cend; ++it)
        rc += QString::fromUtf8(*it);
    return rc;
}

static inline QString charToQString(const char *c)
{
    if (!c)
        return QString::null;
    return QString::fromUtf8(c);
}

namespace  {
    // ------- QDesignerMetaEnum
    class QDesignerMetaEnum : public QDesignerMetaEnumInterface {
    public:
        QDesignerMetaEnum(const QMetaEnum &qEnum);
        virtual bool isFlag() const                        {  return m_enum.isFlag(); }
        virtual QString key(int index) const               { return charToQString(m_enum.key(index)); }
        virtual int keyCount() const                       { return m_enum.keyCount(); }
        virtual int keyToValue(const QString &key) const   { return m_enum.keyToValue(key.toUtf8()); }
        virtual int keysToValue(const QString &keys) const { return m_enum.keysToValue(keys.toUtf8()); }
        virtual QString name() const                       { return m_name; }
        virtual QString scope() const                      { return m_scope; }
        virtual QString separator() const;
        virtual int value(int index) const                 { return m_enum.value(index); }
        virtual QString valueToKey(int value) const        { return charToQString(m_enum.valueToKey(value)); }
        virtual QString valueToKeys(int value) const       { return charToQString(m_enum.valueToKeys(value)); }

    private:
        const QMetaEnum m_enum;
        const QString m_name;
        const QString m_scope;
    };

    QDesignerMetaEnum::QDesignerMetaEnum(const QMetaEnum &qEnum) :
        m_enum(qEnum),
        m_name(charToQString(m_enum.name())),
        m_scope(charToQString(m_enum.scope()))
    {
    }

    QString QDesignerMetaEnum::separator() const
    {
        static const QString rc = QLatin1String("::");
        return rc;
    }

    // ------- QDesignerMetaProperty
    class QDesignerMetaProperty : public QDesignerMetaPropertyInterface {
    public:
        QDesignerMetaProperty(const QMetaProperty &property);
        virtual ~QDesignerMetaProperty();

        virtual const QDesignerMetaEnumInterface *enumerator() const { return m_enumerator; }

        virtual Kind kind() const { return m_kind; }

        virtual AccessFlags accessFlags() const { return m_access; }
        virtual Attributes attributes(const QObject *object = 0) const;

        virtual QVariant::Type type() const { return m_property.type(); }
        virtual QString name() const        { return m_name; }
        virtual QString typeName() const    { return m_typeName; }
        virtual int userType() const        { return m_property.userType(); }
        virtual bool hasSetter() const              { return m_property.hasStdCppSet(); }

        virtual QVariant read(const QObject *object) const { return m_property.read(object); }
        virtual bool reset(QObject *object) const          { return m_property.reset(object); }
        virtual bool write(QObject *object, const QVariant &value) const { return m_property.write(object, value); }

    private:
        const QMetaProperty m_property;
        const QString m_name;
        const QString m_typeName;
        Kind m_kind;
        AccessFlags m_access;
        Attributes m_defaultAttributes;
        QDesignerMetaEnumInterface *m_enumerator;
    };

    QDesignerMetaProperty::QDesignerMetaProperty(const QMetaProperty &property) :
        m_property(property),
        m_name(charToQString(m_property.name())),
        m_typeName(charToQString(m_property.typeName())),
        m_kind(OtherKind),
        m_enumerator(0)
    {
        if (m_property.isFlagType() || m_property.isEnumType()) {
            const QMetaEnum metaEnum = m_property.enumerator();
            Q_ASSERT(metaEnum.isValid());
            m_enumerator = new QDesignerMetaEnum(metaEnum);
        }
        // kind
        if (m_property.isFlagType())
            m_kind = FlagKind;
        else
            if (m_property.isEnumType())
                m_kind = EnumKind;
        // flags and attributes
        if (m_property.isReadable())
            m_access |= ReadAccess;
        if (m_property.isWritable())
            m_access |= WriteAccess;
        if (m_property.isResettable())
            m_access |= ResetAccess;

        if (m_property.isDesignable())
            m_defaultAttributes |= DesignableAttribute;
        if (m_property.isScriptable())
            m_defaultAttributes |= ScriptableAttribute;
        if (m_property.isStored())
            m_defaultAttributes |= StoredAttribute;
        if (m_property.isUser())
            m_defaultAttributes |= UserAttribute;
    }

    QDesignerMetaProperty::~QDesignerMetaProperty()
    {
        delete m_enumerator;
    }

    QDesignerMetaProperty::Attributes QDesignerMetaProperty::attributes(const QObject *object) const
    {
        if (!object)
            return m_defaultAttributes;
        Attributes rc;
        if (m_property.isDesignable(object))
            rc |= DesignableAttribute;
        if (m_property.isScriptable(object))
            rc |= ScriptableAttribute;
        if (m_property.isStored(object))
            rc |= StoredAttribute;
        if (m_property.isUser(object))
            rc |= UserAttribute;
        return rc;
    }

    // -------------- QDesignerMetaMethod

    class QDesignerMetaMethod : public QDesignerMetaMethodInterface {
    public:
        QDesignerMetaMethod(const QMetaMethod &method);

        virtual Access access() const               { return m_access; }
        virtual MethodType methodType() const       { return m_methodType; }
        virtual QStringList parameterNames() const  { return m_parameterNames; }
        virtual QStringList parameterTypes() const  { return m_parameterTypes; }
        virtual QString signature() const           { return m_signature; }
        virtual QString normalizedSignature() const { return m_normalizedSignature; }
        virtual QString tag() const                 { return m_tag; }
        virtual QString typeName() const            { return m_typeName; }

    private:
        Access m_access;
        MethodType m_methodType;
        const QStringList m_parameterNames;
        const QStringList m_parameterTypes;
        const QString m_signature;
        const QString m_normalizedSignature;
        const QString m_tag;
        const QString m_typeName;
    };

    QDesignerMetaMethod::QDesignerMetaMethod(const QMetaMethod &method) :
       m_parameterNames(byteArrayListToStringList(method.parameterNames())),
       m_parameterTypes(byteArrayListToStringList(method.parameterTypes())),
       m_signature(charToQString(method.signature())),
       m_normalizedSignature(charToQString(QMetaObject::normalizedSignature(method.signature()))),
       m_tag(charToQString(method.tag())),
       m_typeName(charToQString(method.typeName()))
    {
        switch (method.access()) {
        case QMetaMethod::Public:
            m_access = Public;
            break;
        case QMetaMethod::Protected:
            m_access = Protected;
            break;
        case QMetaMethod::Private:
            m_access = Private;
            break;

        }
        switch (method.methodType()) {
        case QMetaMethod::Method:
            m_methodType = Method;
            break;
        case QMetaMethod::Signal:
            m_methodType = Signal;
            break;

        case QMetaMethod::Slot:
            m_methodType = Slot;
            break;
        }
    }

    // ------------- QDesignerMetaObject
    class QDesignerMetaObject : public QDesignerMetaObjectInterface {
    public:
        QDesignerMetaObject(const qdesigner_internal::QDesignerIntrospection *introspection, const QMetaObject *metaObject);
        virtual ~QDesignerMetaObject();

        virtual QString className() const { return m_className; }
        virtual const QDesignerMetaEnumInterface *enumerator(int index) const { return m_enumerators[index]; }
        virtual int enumeratorCount() const { return m_enumerators.size(); }
        virtual int enumeratorOffset() const { return m_metaObject->enumeratorOffset(); }

        virtual int indexOfEnumerator(const QString &name) const { return m_metaObject->indexOfEnumerator(name.toUtf8()); }
        virtual int indexOfMethod(const QString &method) const { return m_metaObject->indexOfMethod(method.toUtf8()); }
        virtual int indexOfProperty(const QString &name) const { return m_metaObject->indexOfProperty(name.toUtf8()); }
        virtual int indexOfSignal(const QString &signal) const { return m_metaObject->indexOfSignal(signal.toUtf8());  }
        virtual int indexOfSlot(const QString &slot) const { return m_metaObject->indexOfSlot(slot.toUtf8()); }

        virtual const QDesignerMetaMethodInterface *method(int index) const { return m_methods[index]; }
        virtual int methodCount() const { return m_methods.size(); }
        virtual int methodOffset() const { return m_metaObject->methodOffset(); }

        virtual const QDesignerMetaPropertyInterface *property(int index) const { return m_properties[index]; }
        virtual int propertyCount() const { return m_properties.size(); }
        virtual int propertyOffset() const { return m_metaObject->propertyOffset(); }

        const QDesignerMetaObjectInterface *superClass() const;
        virtual const QDesignerMetaPropertyInterface *userProperty() const { return m_userProperty; }

    private:
        const QString m_className;
        const qdesigner_internal::QDesignerIntrospection *m_introspection;
        const QMetaObject *m_metaObject;

        typedef QVector<QDesignerMetaEnumInterface *> Enumerators;
        Enumerators m_enumerators;

        typedef QVector<QDesignerMetaMethodInterface *> Methods;
        Methods m_methods;

        typedef QVector<QDesignerMetaPropertyInterface *> Properties;
        Properties m_properties;

        QDesignerMetaPropertyInterface *m_userProperty;
    };

    QDesignerMetaObject::QDesignerMetaObject(const qdesigner_internal::QDesignerIntrospection *introspection, const QMetaObject *metaObject) :
       m_className(charToQString(metaObject->className())),
       m_introspection(introspection),
       m_metaObject(metaObject),
       m_userProperty(0)
    {
        const int numEnumerators = metaObject->enumeratorCount();
        m_enumerators.reserve(numEnumerators);
        for (int i = 0; i < numEnumerators; i++)
            m_enumerators.push_back(new QDesignerMetaEnum(metaObject->enumerator(i)));
        const int numMethods = metaObject->methodCount();
        m_methods.reserve(numMethods);
        for (int i = 0; i < numMethods; i++)
            m_methods.push_back(new QDesignerMetaMethod(metaObject->method(i)));

        const int numProperties = metaObject->propertyCount();
        m_properties.reserve(numProperties);
        for (int i = 0; i < numProperties; i++)
            m_properties.push_back(new QDesignerMetaProperty(metaObject->property(i)));

        const QMetaProperty userProperty = metaObject->userProperty();
        if (userProperty.isValid())
            m_userProperty = new QDesignerMetaProperty(userProperty);
    }

    QDesignerMetaObject::~QDesignerMetaObject()
    {
        qDeleteAll(m_enumerators);
        qDeleteAll(m_methods);
        qDeleteAll(m_properties);
        delete m_userProperty;
    }

    const QDesignerMetaObjectInterface *QDesignerMetaObject::superClass() const
    {
        const QMetaObject *qSuperClass = m_metaObject->superClass();
        if (!qSuperClass)
            return 0;
        return m_introspection->metaObjectForQMetaObject(qSuperClass);
    }

}

namespace qdesigner_internal {

    QDesignerIntrospection::QDesignerIntrospection()
    {
    }

    QDesignerIntrospection::~QDesignerIntrospection()
    {
        qDeleteAll(m_metaObjectMap.values());
    }

    const QDesignerMetaObjectInterface* QDesignerIntrospection::metaObject(const QObject *object) const
    {
        return metaObjectForQMetaObject(object->metaObject());
    }

    const QDesignerMetaObjectInterface* QDesignerIntrospection::metaObjectForQMetaObject(const QMetaObject *metaObject) const
    {
        MetaObjectMap::iterator it = m_metaObjectMap.find(metaObject);
        if (it == m_metaObjectMap.end())
            it = m_metaObjectMap.insert(metaObject, new QDesignerMetaObject(this, metaObject));
        return it.value();
    }
}

QT_END_NAMESPACE
