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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Designer.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef ABSTRACTMETAOBJECT_H
#define ABSTRACTMETAOBJECT_H

#include <QtDesigner/sdk_global.h>
#include <QtCore/QVariant>
#include <QtCore/QFlags>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QDESIGNER_SDK_EXPORT QDesignerMetaEnumInterface
{
public:
    QDesignerMetaEnumInterface();
    virtual ~QDesignerMetaEnumInterface();
    virtual bool isFlag() const = 0;
    virtual QString key(int index) const = 0;
    virtual int keyCount() const = 0;
    virtual int keyToValue(const QString &key) const = 0;
    virtual int keysToValue(const QString &keys) const = 0;
    virtual QString name() const = 0;
    virtual QString scope() const = 0;
    virtual QString separator() const = 0;
    virtual int value(int index) const = 0;
    virtual QString valueToKey(int value) const = 0;
    virtual QString valueToKeys(int value) const  = 0;
};

class QDESIGNER_SDK_EXPORT QDesignerMetaPropertyInterface
{
public:
    enum Kind { EnumKind, FlagKind, OtherKind };
    enum AccessFlag { ReadAccess = 0x0001, WriteAccess = 0x0002, ResetAccess = 0x0004 };
    enum Attribute { DesignableAttribute = 0x0001, ScriptableAttribute = 0x0002, StoredAttribute = 0x0004, UserAttribute = 0x0008};
    Q_DECLARE_FLAGS(Attributes, Attribute)
    Q_DECLARE_FLAGS(AccessFlags, AccessFlag)

    QDesignerMetaPropertyInterface();
    virtual ~QDesignerMetaPropertyInterface();

    virtual const QDesignerMetaEnumInterface *enumerator() const = 0;

    virtual Kind kind() const = 0;
    virtual AccessFlags accessFlags() const = 0;
    virtual Attributes attributes(const QObject *object = 0) const = 0;

    virtual QVariant::Type type() const = 0;
    virtual QString name() const = 0;
    virtual QString typeName() const = 0;
    virtual int userType() const = 0;
    virtual bool hasSetter() const = 0;

    virtual QVariant read(const QObject *object) const = 0;
    virtual bool reset(QObject *object) const = 0;
    virtual bool write(QObject *object, const QVariant &value) const  = 0;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QDesignerMetaPropertyInterface::AccessFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(QDesignerMetaPropertyInterface::Attributes)

class QDESIGNER_SDK_EXPORT QDesignerMetaMethodInterface
{
public:
    QDesignerMetaMethodInterface();
    virtual ~QDesignerMetaMethodInterface();

    enum MethodType { Method, Signal, Slot };
    enum Access { Private, Protected, Public };

    virtual Access access() const  = 0;
    virtual MethodType methodType() const = 0;
    virtual QStringList parameterNames() const = 0;
    virtual QStringList parameterTypes() const = 0;
    virtual QString signature() const = 0;
    virtual QString normalizedSignature() const = 0;
    virtual QString tag() const = 0;
    virtual QString typeName() const  = 0;
};

class QDESIGNER_SDK_EXPORT QDesignerMetaObjectInterface {
public:
    QDesignerMetaObjectInterface();
    virtual ~QDesignerMetaObjectInterface();

    virtual QString className() const = 0;
    virtual const QDesignerMetaEnumInterface *enumerator(int index) const = 0;
    virtual int enumeratorCount() const = 0;
    virtual int enumeratorOffset() const = 0;

    virtual int indexOfEnumerator(const QString &name) const = 0;
    virtual int indexOfMethod(const QString &method) const = 0;
    virtual int indexOfProperty(const QString &name) const = 0;
    virtual int indexOfSignal(const QString &signal) const = 0;
    virtual int indexOfSlot(const QString &slot) const = 0;

    virtual const QDesignerMetaMethodInterface *method(int index) const = 0;
    virtual int methodCount() const = 0;
    virtual int methodOffset() const = 0;

    virtual const  QDesignerMetaPropertyInterface *property(int index) const = 0;
    virtual int propertyCount() const = 0;
    virtual int propertyOffset() const = 0;

    virtual const QDesignerMetaObjectInterface *superClass() const = 0;
    virtual const QDesignerMetaPropertyInterface *userProperty() const  = 0;
};

// To be obtained from core
class QDESIGNER_SDK_EXPORT QDesignerIntrospectionInterface {
public:
    QDesignerIntrospectionInterface();
    virtual ~QDesignerIntrospectionInterface();

    virtual const QDesignerMetaObjectInterface* metaObject(const QObject *object) const = 0;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // ABSTRACTMETAOBJECT_H
