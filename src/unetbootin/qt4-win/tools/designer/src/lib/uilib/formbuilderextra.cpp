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

#include "formbuilderextra_p.h"
#include "abstractformbuilder.h"
#include "resourcebuilder_p.h"

#include <QtCore/QVariant>
#include <QtGui/QLabel>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

#ifdef QFORMINTERNAL_NAMESPACE
namespace QFormInternal {
#endif

void uiLibWarning(const QString &message) {
    qWarning("Designer: %s", qPrintable(message));
}

QFormBuilderExtra::QFormBuilderExtra() :
    m_layoutWidget(false),
    m_resourceBuilder(0)
{
}

QFormBuilderExtra::~QFormBuilderExtra()
{
    clearResourceBuilder();
}

void QFormBuilderExtra::clear()
{
    m_buddies.clear();
    m_rootWidget = 0;
#ifndef QT_FORMBUILDER_NO_SCRIPT
    m_FormScriptRunner.clearErrors();
    m_customWidgetScriptHash.clear();
#endif
}

bool QFormBuilderExtra::applyPropertyInternally(QObject *o, const QString &propertyName, const QVariant &value)
{
    // Store buddies and apply them later on as the widgets might not exist yet.
    QLabel *label = qobject_cast<QLabel*>(o);
    if (!label || propertyName != QFormBuilderStrings::instance().buddyProperty)
        return false;

    m_buddies.insert(label, value.toString());
    return true;
}

void QFormBuilderExtra::applyInternalProperties() const
{
    if (m_buddies.empty())
        return;

    const BuddyHash::const_iterator cend = m_buddies.constEnd();
    for (BuddyHash::const_iterator it = m_buddies.constBegin(); it != cend; ++it )
        applyBuddy(it.value(), BuddyApplyAll, it.key());
}

bool QFormBuilderExtra::applyBuddy(const QString &buddyName, BuddyMode applyMode, QLabel *label)
{
    if (buddyName.isEmpty()) {
        label->setBuddy(0);
        return false;
    }

    const QWidgetList widgets = qFindChildren<QWidget*>(label->topLevelWidget(), buddyName);
    if (widgets.empty()) {
        label->setBuddy(0);
        return false;
    }

    const QWidgetList::const_iterator cend = widgets.constEnd();
    for ( QWidgetList::const_iterator it =  widgets.constBegin(); it !=  cend; ++it) {
        if (applyMode == BuddyApplyAll || !(*it)->isHidden()) {
            label->setBuddy(*it);
            return true;
        }
    }

    label->setBuddy(0);
    return false;
}

const QPointer<QWidget> &QFormBuilderExtra::rootWidget() const
{
    return m_rootWidget;
}

void QFormBuilderExtra::setRootWidget(const QPointer<QWidget> &w)
{
    m_rootWidget = w;
}

#ifndef QT_FORMBUILDER_NO_SCRIPT
QFormScriptRunner &QFormBuilderExtra::formScriptRunner()
{
    return m_FormScriptRunner;
}

void QFormBuilderExtra::storeCustomWidgetScript(const QString &className, const QString &script)
{
    m_customWidgetScriptHash.insert(className, script);
}

QString QFormBuilderExtra::customWidgetScript(const QString &className) const
{
    const CustomWidgetScriptHash::const_iterator it = m_customWidgetScriptHash.constFind(className);
    if ( it == m_customWidgetScriptHash.constEnd())
        return QString();
    return it.value();
}

#endif

void QFormBuilderExtra::storeCustomWidgetBaseClass(const QString &className, const QString &baseClassName)
{
    m_customWidgetBaseClassHash.insert(className, baseClassName);
}

QString QFormBuilderExtra::customWidgetBaseClass(const QString &className) const
{
    const QHash<QString, QString>::const_iterator it = m_customWidgetBaseClassHash.constFind(className);
    if (it == m_customWidgetBaseClassHash.constEnd())
        return QString();
    return it.value();
}

void QFormBuilderExtra::storeCustomWidgetAddPageMethod(const QString &className, const QString &ct)
{
    m_customWidgetAddPageMethodHash.insert(className, ct);
}

QString QFormBuilderExtra::customWidgetAddPageMethod(const QString &className) const
{
    const QHash<QString, QString>::const_iterator it = m_customWidgetAddPageMethodHash.constFind(className);
    if (it == m_customWidgetAddPageMethodHash.constEnd())
        return QString();
    return it.value();
}

namespace {
    typedef QHash<const QAbstractFormBuilder *, QFormBuilderExtra *> FormBuilderPrivateHash;
}

Q_GLOBAL_STATIC(FormBuilderPrivateHash, g_FormBuilderPrivateHash)

QFormBuilderExtra *QFormBuilderExtra::instance(const QAbstractFormBuilder *afb)
{
    FormBuilderPrivateHash &fbHash = *g_FormBuilderPrivateHash();

    FormBuilderPrivateHash::iterator it = fbHash.find(afb);
    if (it == fbHash.end())
        it = fbHash.insert(afb, new QFormBuilderExtra);
    return it.value();
}

void QFormBuilderExtra::removeInstance(const QAbstractFormBuilder *afb)
{
    FormBuilderPrivateHash &fbHash = *g_FormBuilderPrivateHash();

    FormBuilderPrivateHash::iterator it = fbHash.find(afb);
    if (it != fbHash.end()) {
        delete it.value();
        fbHash.erase(it);
    }
}

void QFormBuilderExtra::setProcessingLayoutWidget(bool processing)
{
    m_layoutWidget = processing;
}

 bool QFormBuilderExtra::processingLayoutWidget() const
{
    return m_layoutWidget;
}
void QFormBuilderExtra::setResourceBuilder(QResourceBuilder *builder)
{
    if (m_resourceBuilder == builder)
        return;
    clearResourceBuilder();
    m_resourceBuilder = builder;
}

QResourceBuilder *QFormBuilderExtra::resourceBuilder() const
{
    return m_resourceBuilder;
}

void QFormBuilderExtra::clearResourceBuilder()
{
    if (m_resourceBuilder) {
        delete m_resourceBuilder;
        m_resourceBuilder = 0;
    }
}

// ------------ QFormBuilderStrings

QFormBuilderStrings::QFormBuilderStrings() :
    buddyProperty(QLatin1String("buddy")),
    cursorProperty(QLatin1String("cursor")),
    objectNameProperty(QLatin1String("objectName")),
    trueValue(QLatin1String("true")),
    falseValue(QLatin1String("false")),
    horizontalPostFix(QLatin1String("Horizontal")),
    separator(QLatin1String("separator")),
    defaultTitle(QLatin1String("Page")),
    titleAttribute(QLatin1String("title")),
    labelAttribute(QLatin1String("label")),
    toolTipAttribute(QLatin1String("toolTip")),
    iconAttribute(QLatin1String("icon")),
    pixmapAttribute(QLatin1String("pixmap")),
    textAttribute(QLatin1String("text")),
    currentIndexProperty(QLatin1String("currentIndex")),
    toolBarAreaAttribute(QLatin1String("toolBarArea")),
    toolBarBreakAttribute(QLatin1String("toolBarBreak")),
    dockWidgetAreaAttribute(QLatin1String("dockWidgetArea")),
    marginProperty(QLatin1String("margin")),
    spacingProperty(QLatin1String("spacing")),
    leftMarginProperty(QLatin1String("leftMargin")),
    topMarginProperty(QLatin1String("topMargin")),
    rightMarginProperty(QLatin1String("rightMargin")),
    bottomMarginProperty(QLatin1String("bottomMargin")),
    horizontalSpacingProperty(QLatin1String("horizontalSpacing")),
    verticalSpacingProperty(QLatin1String("verticalSpacing")),
    sizeHintProperty(QLatin1String("sizeHint")),
    sizeTypeProperty(QLatin1String("sizeType")),
    orientationProperty(QLatin1String("orientation")),
    styleSheetProperty(QLatin1String("styleSheet")),
    qtHorizontal(QLatin1String("Qt::Horizontal")),
    qtVertical(QLatin1String("Qt::Vertical")),
    currentRowProperty(QLatin1String("currentRow")),
    tabSpacingProperty(QLatin1String("tabSpacing")),
    qWidgetClass(QLatin1String("QWidget")),
    lineClass(QLatin1String("Line")),
    geometryProperty(QLatin1String("geometry")),
    scriptWidgetVariable(QLatin1String("widget")),
    scriptChildWidgetsVariable(QLatin1String("childWidgets"))
{
}

const QFormBuilderStrings &QFormBuilderStrings::instance()
{
    static const QFormBuilderStrings rc;
    return rc;
}

#ifdef QFORMINTERNAL_NAMESPACE
} // namespace QFormInternal
#endif

QT_END_NAMESPACE
