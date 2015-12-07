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

#include "brushpropertymanager.h"
#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "qtpropertybrowserutils_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QVariant>
#include <QtCore/QString>

static const char *brushStyles[] = {
QT_TRANSLATE_NOOP("BrushPropertyManager", "No brush"),
QT_TRANSLATE_NOOP("BrushPropertyManager", "Solid"),
QT_TRANSLATE_NOOP("BrushPropertyManager", "Dense 1"),
QT_TRANSLATE_NOOP("BrushPropertyManager", "Dense 2"),
QT_TRANSLATE_NOOP("BrushPropertyManager", "Dense 3"),
QT_TRANSLATE_NOOP("BrushPropertyManager", "Dense 4"),
QT_TRANSLATE_NOOP("BrushPropertyManager", "Dense 5"),
QT_TRANSLATE_NOOP("BrushPropertyManager", "Dense 6"),
QT_TRANSLATE_NOOP("BrushPropertyManager", "Dense 7"),
QT_TRANSLATE_NOOP("BrushPropertyManager", "Horizontal"),
QT_TRANSLATE_NOOP("BrushPropertyManager", "Vertical"),
QT_TRANSLATE_NOOP("BrushPropertyManager", "Cross"),
QT_TRANSLATE_NOOP("BrushPropertyManager", "Backward diagonal"),
QT_TRANSLATE_NOOP("BrushPropertyManager", "Forward diagonal"),
QT_TRANSLATE_NOOP("BrushPropertyManager", "Crossing diagonal"),
QT_TRANSLATE_NOOP("BrushPropertyManager", "Linear gradient"),
QT_TRANSLATE_NOOP("BrushPropertyManager", "Conical gradient"),
QT_TRANSLATE_NOOP("BrushPropertyManager", "Radial gradient"),
QT_TRANSLATE_NOOP("BrushPropertyManager", "Texture")
};

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

BrushPropertyManager::BrushPropertyManager()
{
}

int BrushPropertyManager::brushStyleToIndex(Qt::BrushStyle st)
{
    switch (st) {
    case Qt::NoBrush:       return 0;
    case Qt::SolidPattern:  return 1;
    case Qt::Dense1Pattern: return 2;
    case Qt::Dense2Pattern: return 3;
    case Qt::Dense3Pattern: return 4;
    case Qt::Dense4Pattern: return 5;
    case Qt::Dense5Pattern: return 6;
    case Qt::Dense6Pattern: return 7;
    case Qt::Dense7Pattern: return 8;
    case Qt::HorPattern:    return 9;
    case Qt::VerPattern:    return 10;
    case Qt::CrossPattern:  return 11;
    case Qt::BDiagPattern:  return 12;
    case Qt::FDiagPattern:  return 13;
    case Qt::DiagCrossPattern:       return 14;
    case Qt::LinearGradientPattern:  return 15;
    case Qt::ConicalGradientPattern: return 16;
    case Qt::RadialGradientPattern:  return 17;
    case Qt::TexturePattern:         return 18;
    }
    return 0;
}

Qt::BrushStyle BrushPropertyManager::brushStyleIndexToStyle(int brushStyleIndex)
{
    switch (brushStyleIndex) {
    case  0: return Qt::NoBrush;
    case  1: return Qt::SolidPattern;
    case  2: return Qt::Dense1Pattern;
    case  3: return Qt::Dense2Pattern;
    case  4: return Qt::Dense3Pattern;
    case  5: return Qt::Dense4Pattern;
    case  6: return Qt::Dense5Pattern;
    case  7: return Qt::Dense6Pattern;
    case  8: return Qt::Dense7Pattern;
    case  9: return Qt::HorPattern;
    case 10: return Qt::VerPattern;
    case 11: return Qt::CrossPattern;
    case 12: return Qt::BDiagPattern;
    case 13: return Qt::FDiagPattern;
    case 14: return Qt::DiagCrossPattern;
    case 15: return Qt::LinearGradientPattern;
    case 16: return Qt::ConicalGradientPattern;
    case 17: return Qt::RadialGradientPattern;
    case 18: return Qt::TexturePattern;
    }
    return Qt::NoBrush;
}

const BrushPropertyManager::EnumIndexIconMap &BrushPropertyManager::brushStyleIcons()
{
    // Create a map of icons for the brush style editor
    static EnumIndexIconMap rc;
    if (rc.empty()) {
        const int brushStyleCount = sizeof(brushStyles)/sizeof(const char *);
        QBrush brush(Qt::black);
        const QIcon solidIcon = QtPropertyBrowserUtils::brushValueIcon(brush);
        for (int i = 0; i < brushStyleCount; i++) {
            const Qt::BrushStyle style = brushStyleIndexToStyle(i);
            switch (style) {
            case Qt::SolidPattern:
            case Qt::LinearGradientPattern: // Requires parameter settings
            case Qt::ConicalGradientPattern:
            case Qt::RadialGradientPattern:
            case Qt::TexturePattern:
                rc.insert(i, solidIcon);
                break;
            default:
                brush.setStyle(style);
                rc.insert(i, QtPropertyBrowserUtils::brushValueIcon(brush));
            }
        }
    }
    return rc;
}

QString BrushPropertyManager::brushStyleIndexToString(int brushStyleIndex)
{
    const int brushStyleCount = sizeof(brushStyles)/sizeof(const char *);
    return brushStyleIndex < brushStyleCount ? QCoreApplication::translate("BrushPropertyManager", brushStyles[brushStyleIndex]) :  QString();
}

void BrushPropertyManager::initializeProperty(QtVariantPropertyManager *vm, QtProperty *property, int enumTypeId)
{
    m_brushValues.insert(property, QBrush());
    // style
    QtVariantProperty *styleSubProperty = vm->addProperty(enumTypeId, QCoreApplication::translate("BrushPropertyManager", "Style"));
    property->addSubProperty(styleSubProperty);
    QStringList styles;
    const int brushStyleCount = sizeof(brushStyles)/sizeof(const char *);
    for (int i = 0; i < brushStyleCount; i++)
        styles.push_back(QCoreApplication::translate("BrushPropertyManager", brushStyles[i]));
    styleSubProperty->setAttribute(QLatin1String("enumNames"), styles);
    styleSubProperty->setAttribute(QLatin1String("enumIcons"), qVariantFromValue(brushStyleIcons()));
    m_brushPropertyToStyleSubProperty.insert(property, styleSubProperty);
    m_brushStyleSubPropertyToProperty.insert(styleSubProperty, property);
    // color
    QtVariantProperty *colorSubProperty = vm->addProperty(QVariant::Color, QCoreApplication::translate("BrushPropertyManager", "Color"));
    property->addSubProperty(colorSubProperty);
    m_brushPropertyToColorSubProperty.insert(property, colorSubProperty);
    m_brushColorSubPropertyToProperty.insert(colorSubProperty, property);
}

bool BrushPropertyManager::uninitializeProperty(QtProperty *property)
{
    const PropertyBrushMap::iterator brit = m_brushValues.find(property); // Brushes
    if (brit == m_brushValues.end())
        return false;
    m_brushValues.erase(brit);
    // style
    PropertyToPropertyMap::iterator subit = m_brushPropertyToStyleSubProperty.find(property);
    Q_ASSERT(subit != m_brushPropertyToStyleSubProperty.end());
    m_brushStyleSubPropertyToProperty.remove(subit.value());
    m_brushPropertyToStyleSubProperty.erase(subit);
    // color
    subit = m_brushPropertyToColorSubProperty.find(property);
    Q_ASSERT(subit != m_brushPropertyToColorSubProperty.end());
    m_brushColorSubPropertyToProperty.remove(subit.value());
    m_brushPropertyToColorSubProperty.erase(subit);
    return true;
}

BrushPropertyManager::ValueChangedResult BrushPropertyManager::valueChanged(QtVariantPropertyManager *vm, QtProperty *property, const QVariant &value)
{
    switch (value.type()) {
    case QVariant::Int: // Style subproperty?
        if (QtProperty *brushProperty = m_brushStyleSubPropertyToProperty.value(property, 0)) {
            const QBrush oldValue = m_brushValues.value(brushProperty);
            QBrush newBrush = oldValue;
            const int index = value.toInt();
            newBrush.setStyle(brushStyleIndexToStyle(index));
            if (newBrush == oldValue)
                return Unchanged;
            vm->variantProperty(brushProperty)->setValue(newBrush);
            return Changed;
        }
        break;
    case QVariant::Color: // Color  subproperty?
        if (QtProperty *brushProperty = m_brushColorSubPropertyToProperty.value(property, 0)) {
            const QBrush oldValue = m_brushValues.value(brushProperty);
            QBrush newBrush = oldValue;
            newBrush.setColor(qvariant_cast<QColor>(value));
            if (newBrush == oldValue)
                return Unchanged;
            vm->variantProperty(brushProperty)->setValue(newBrush);
            return Changed;
        }
        break;
    default:
        break;
    }
    return NoMatch;
}

BrushPropertyManager::ValueChangedResult BrushPropertyManager::setValue(QtVariantPropertyManager *vm, QtProperty *property, const QVariant &value)
{
    if (value.type() != QVariant::Brush)
        return NoMatch;
    const PropertyBrushMap::iterator brit = m_brushValues.find(property);
    if (brit == m_brushValues.end())
        return NoMatch;

    const QBrush newBrush = qvariant_cast<QBrush>(value);
    if (newBrush == brit.value())
        return Unchanged;
    brit.value() = newBrush;
    if (QtProperty *styleProperty = m_brushPropertyToStyleSubProperty.value(property))
        vm->variantProperty(styleProperty)->setValue(brushStyleToIndex(newBrush.style()));
    if (QtProperty *colorProperty = m_brushPropertyToColorSubProperty.value(property))
        vm->variantProperty(colorProperty)->setValue(newBrush.color());

    return Changed;
}

bool BrushPropertyManager::valueText(const QtProperty *property, QString *text) const
{
    const PropertyBrushMap::const_iterator brit = m_brushValues.constFind(const_cast<QtProperty *>(property));
    if (brit == m_brushValues.constEnd())
        return false;
    const QBrush &brush = brit.value();
    const QString styleName = brushStyleIndexToString(brushStyleToIndex(brush.style()));
    *text = QCoreApplication::translate("BrushPropertyManager", "[%1, %2]").arg(styleName).arg(QtPropertyBrowserUtils::colorValueText(brush.color()));
    return true;
}

bool BrushPropertyManager::valueIcon(const QtProperty *property, QIcon *icon) const
{
    const PropertyBrushMap::const_iterator brit = m_brushValues.constFind(const_cast<QtProperty *>(property));
    if (brit == m_brushValues.constEnd())
        return false;
    *icon = QtPropertyBrowserUtils::brushValueIcon(brit.value());
    return true;
}

bool BrushPropertyManager::value(const QtProperty *property, QVariant *v) const
{
    const PropertyBrushMap::const_iterator brit = m_brushValues.constFind(const_cast<QtProperty *>(property));
    if (brit == m_brushValues.constEnd())
        return false;
    qVariantSetValue(*v, brit.value());
    return true;
}
}

QT_END_NAMESPACE
