/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
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

#ifndef UILIBPROPERTIES_H
#define UILIBPROPERTIES_H

#include <QtDesigner/uilib_global.h>

#include <QtCore/QObject>
#include <QtCore/QMetaProperty>
#include <QtCore/QLocale>
#include <QtGui/QWidget>

#include "formbuilderextra_p.h"

QT_BEGIN_NAMESPACE

#ifdef QFORMINTERNAL_NAMESPACE
namespace QFormInternal
{
#endif

class QAbstractFormBuilder;
class DomProperty;

QDESIGNER_UILIB_EXPORT DomProperty *variantToDomProperty(QAbstractFormBuilder *abstractFormBuilder, QObject *object, const QString &propertyName, const QVariant &value);


QDESIGNER_UILIB_EXPORT QVariant domPropertyToVariant(const DomProperty *property);
QDESIGNER_UILIB_EXPORT QVariant domPropertyToVariant(QAbstractFormBuilder *abstractFormBuilder, const QMetaObject *meta, const  DomProperty *property);

// This class exists to provide meta information
// for enumerations only.
class QAbstractFormBuilderGadget: public QWidget
{
    Q_OBJECT
    Q_PROPERTY(Qt::Orientation orientation READ fakeOrientation)
    Q_PROPERTY(QSizePolicy::Policy sizeType READ fakeSizeType)
    Q_PROPERTY(QPalette::ColorRole colorRole READ fakeColorRole)
    Q_PROPERTY(QPalette::ColorGroup colorGroup READ fakeColorGroup)
    Q_PROPERTY(QFont::StyleStrategy styleStrategy READ fakeStyleStrategy)
    Q_PROPERTY(Qt::CursorShape cursorShape READ fakeCursorShape)
    Q_PROPERTY(Qt::BrushStyle brushStyle READ fakeBrushStyle)
    Q_PROPERTY(Qt::ToolBarArea toolBarArea READ fakeToolBarArea)
    Q_PROPERTY(QGradient::Type gradientType READ fakeGradientType)
    Q_PROPERTY(QGradient::Spread gradientSpread READ fakeGradientSpread)
    Q_PROPERTY(QGradient::CoordinateMode gradientCoordinate READ fakeGradientCoordinate)
    Q_PROPERTY(QLocale::Language language READ fakeLanguage)
    Q_PROPERTY(QLocale::Country country READ fakeCountry)
public:
    QAbstractFormBuilderGadget() { Q_ASSERT(0); }

    Qt::Orientation fakeOrientation() const     { Q_ASSERT(0); return Qt::Horizontal; }
    QSizePolicy::Policy fakeSizeType() const    { Q_ASSERT(0); return QSizePolicy::Expanding; }
    QPalette::ColorGroup fakeColorGroup() const { Q_ASSERT(0); return static_cast<QPalette::ColorGroup>(0); }
    QPalette::ColorRole fakeColorRole() const   { Q_ASSERT(0); return static_cast<QPalette::ColorRole>(0); }
    QFont::StyleStrategy fakeStyleStrategy() const     { Q_ASSERT(0); return QFont::PreferDefault; }
    Qt::CursorShape fakeCursorShape() const     { Q_ASSERT(0); return Qt::ArrowCursor; }
    Qt::BrushStyle fakeBrushStyle() const       { Q_ASSERT(0); return Qt::NoBrush; }
    Qt::ToolBarArea fakeToolBarArea() const {  Q_ASSERT(0); return Qt::NoToolBarArea; }
    QGradient::Type fakeGradientType() const    { Q_ASSERT(0); return QGradient::NoGradient; }
    QGradient::Spread fakeGradientSpread() const  { Q_ASSERT(0); return QGradient::PadSpread; }
    QGradient::CoordinateMode fakeGradientCoordinate() const  { Q_ASSERT(0); return QGradient::LogicalMode; }
    QLocale::Language fakeLanguage() const  { Q_ASSERT(0); return QLocale::C; }
    QLocale::Country fakeCountry() const  { Q_ASSERT(0); return QLocale::AnyCountry; }
};

// Convert key to value for a given QMetaEnum
template <class EnumType>
inline EnumType enumKeyToValue(const QMetaEnum &metaEnum,const char *key, const EnumType* = 0)
{
    int val = metaEnum.keyToValue(key);
    if (val == -1) {

        uiLibWarning(QObject::tr("The enumeration-value \"%1\" is invalid. The default value \"%2\" will be used instead.")
                    .arg(QString::fromUtf8(key)).arg(QString::fromUtf8(metaEnum.key(0))));
        val = metaEnum.value(0);
    }
    return static_cast<EnumType>(val);
}

// Access meta enumeration object of a qobject
template <class QObjectType>
inline QMetaEnum metaEnum(const char *name, const QObjectType* = 0)
{
    const int e_index = QObjectType::staticMetaObject.indexOfProperty(name);
    Q_ASSERT(e_index != -1);
    return QObjectType::staticMetaObject.property(e_index).enumerator();
}

// Convert key to value for enumeration by name
template <class QObjectType, class EnumType>
inline EnumType enumKeyOfObjectToValue(const char *enumName, const char *key, const QObjectType* = 0, const EnumType* = 0)
{
    const QMetaEnum me = metaEnum<QObjectType>(enumName);
    return enumKeyToValue<EnumType>(me, key);
}

#ifdef QFORMINTERNAL_NAMESPACE
}
#endif

QT_END_NAMESPACE

#endif // UILIBPROPERTIES_H
