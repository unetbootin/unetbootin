/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsimpleresource_p.h"
#include "widgetfactory_p.h"

#include <formscriptrunner_p.h>
#include <properties_p.h>
#include <ui4_p.h>

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerLanguageExtension>
#include <script_p.h>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerCustomWidgetInterface>
#include <QtDesigner/extrainfo.h>

#include <QtGui/QIcon>
#include <QtGui/QWidget>
#include <QtGui/QAction>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

namespace {
    typedef QList<DomWidgetData*> DomWidgetDataList;
    typedef QList<DomProperty*> DomPropertyList;
    typedef QList<QDesignerCustomWidgetInterface *> CustomWidgetInterfaces;
}

namespace qdesigner_internal {

bool QSimpleResource::m_warningsEnabled = true;

QSimpleResource::QSimpleResource(QDesignerFormEditorInterface *core) :
    QAbstractFormBuilder(),
    m_core(core)
{
    QString workingDirectory = QDir::homePath();
    workingDirectory +=  QDir::separator();
    workingDirectory +=  QLatin1String(".designer");
    setWorkingDirectory(QDir(workingDirectory));
    // Disable scripting in the editors.
    formScriptRunner()-> setOptions(QFormScriptRunner::DisableScripts);
}

QSimpleResource::~QSimpleResource()
{

}

QBrush QSimpleResource::setupBrush(DomBrush *brush)
{
    return QAbstractFormBuilder::setupBrush(brush);
}

DomBrush *QSimpleResource::saveBrush(const QBrush &brush)
{
    return QAbstractFormBuilder::saveBrush(brush);
}

QIcon QSimpleResource::nameToIcon(const QString &filePath, const QString &qrcPath)
{
    Q_UNUSED(filePath)
    Q_UNUSED(qrcPath)
    qWarning() << "QSimpleResource::nameToIcon() is obsoleted";
    return QIcon();
}

QString QSimpleResource::iconToFilePath(const QIcon &pm) const
{
    Q_UNUSED(pm)
    qWarning() << "QSimpleResource::iconToFilePath() is obsoleted";
    return QString();
}

QString QSimpleResource::iconToQrcPath(const QIcon &pm) const
{
    Q_UNUSED(pm)
    qWarning() << "QSimpleResource::iconToQrcPath() is obsoleted";
    return QString();
}

QPixmap QSimpleResource::nameToPixmap(const QString &filePath, const QString &qrcPath)
{
    Q_UNUSED(filePath)
    Q_UNUSED(qrcPath)
    qWarning() << "QSimpleResource::nameToPixmap() is obsoleted";
    return QPixmap();
}

QString QSimpleResource::pixmapToFilePath(const QPixmap &pm) const
{
    Q_UNUSED(pm)
    qWarning() << "QSimpleResource::pixmapToFilePath() is obsoleted";
    return QString();
}

QString QSimpleResource::pixmapToQrcPath(const QPixmap &pm) const
{
    Q_UNUSED(pm)
    qWarning() << "QSimpleResource::pixmapToQrcPath() is obsoleted";
    return QString();
}

DomScript *QSimpleResource::createScript(const QString &script, ScriptSource source)
{
    if (script.isEmpty())
        return 0;
    DomScript *domScript = new DomScript();
    switch (source) {
    case ScriptExtension:
        domScript->setAttributeSource(QLatin1String("extension"));
        break;
    case ScriptDesigner:
        domScript->setAttributeSource(QLatin1String("designer"));
        break;
    case ScriptCustomWidgetPlugin:
        domScript->setAttributeSource(QLatin1String("customwidgetplugin"));
        break;
    }
    domScript->setAttributeLanguage(QLatin1String("Qt Script"));
    domScript->setText(script);
    return domScript;
}

// Add a script to a list of DomScripts unless empty
void QSimpleResource::addScript(const QString &script, ScriptSource source, DomScripts &domScripts)
{
    if (DomScript *domScript = createScript(script, source)) {
        domScripts += domScript;
    }
}

void QSimpleResource::addExtensionDataToDOM(QAbstractFormBuilder *afb,
                                            QDesignerFormEditorInterface *core,
                                            DomWidget *ui_widget, QWidget *widget)
{
    QExtensionManager *emgr = core->extensionManager();
    if (QDesignerExtraInfoExtension *extra = qt_extension<QDesignerExtraInfoExtension*>(emgr, widget)) {
        extra->saveWidgetExtraInfo(ui_widget);
    }
    if (QDesignerScriptExtension *scriptExt = qt_extension<QDesignerScriptExtension*>(emgr, widget)) {
        // Add internal state
        const QVariantMap data = scriptExt->data();
        if (!data.empty()) {
            // Convert the map to a DomState.
            // We pass on the widget for property introspection. Thus, non-designable properties
            // that have to be converted using QMetaObject (enums and the like) will work.
            DomPropertyList properties;
            const QVariantMap::const_iterator vcend = data.constEnd();
            for (QVariantMap::const_iterator it = data.constBegin(); it != vcend; ++it) {
                if (DomProperty *prop = variantToDomProperty(afb, widget->metaObject(), it.key(), it.value()))
                    properties += prop;
            }
            if (!properties.empty()) {
                DomWidgetData *domData = new DomWidgetData;
                domData->setElementProperty(properties);
                DomWidgetDataList domDataList;
                 domDataList += domData;
                ui_widget->setElementWidgetData(domDataList);
            }

        }
        // Add script
        const QString script = scriptExt->script();
        if (!script.isEmpty()) {
            DomScripts domScripts = ui_widget->elementScript();
            addScript(script, ScriptExtension, domScripts);
            ui_widget->setElementScript(domScripts);
        }
    }
}

void QSimpleResource::applyExtensionDataFromDOM(QAbstractFormBuilder *afb,
                                                QDesignerFormEditorInterface *core,
                                                DomWidget *ui_widget, QWidget *widget, bool applyState)
{
    QExtensionManager *emgr = core->extensionManager();
    if (QDesignerExtraInfoExtension *extra = qt_extension<QDesignerExtraInfoExtension*>(emgr, widget)) {
        extra->loadWidgetExtraInfo(ui_widget);
    }
    if (applyState) {
        if (QDesignerScriptExtension *scriptExt = qt_extension<QDesignerScriptExtension*>(emgr, widget)) {
            // Apply the state.
            // We pass on the widget for property introspection. Thus, non-designable properties
            // that have to be converted using QMetaObject (enums and the like) will work.
            QVariantMap data;
            DomWidgetDataList domDataList = ui_widget->elementWidgetData();
            if (!domDataList.empty()) {
                foreach (const DomWidgetData *domData, domDataList) {
                    const DomPropertyList properties = domData->elementProperty();
                    foreach(const DomProperty *prop, properties) {
                        const QVariant vprop = domPropertyToVariant(afb, widget->metaObject(), prop);
                        if (vprop.type() != QVariant::Invalid)
                            data.insert(prop->attributeName(), vprop);
                    }
                }
            }
            scriptExt->setData(data);
        }
    }
}

QString QSimpleResource::customWidgetScript(QDesignerFormEditorInterface *core, QObject *object)
{
    return customWidgetScript(core, qdesigner_internal::WidgetFactory::classNameOf(core, object));
}

bool QSimpleResource::hasCustomWidgetScript(QDesignerFormEditorInterface *, QObject *)
{
    return false;
}

QString QSimpleResource::customWidgetScript(QDesignerFormEditorInterface *, const QString &)
{
    return QString();
}

bool QSimpleResource::setWarningsEnabled(bool warningsEnabled)
{
    const bool rc = m_warningsEnabled;
    m_warningsEnabled = warningsEnabled;
    return rc;
}

bool QSimpleResource::warningsEnabled()
{
    return m_warningsEnabled;
}

// ------------ FormBuilderClipboard

FormBuilderClipboard::FormBuilderClipboard(QWidget *w)
{
    m_widgets += w;
}

bool FormBuilderClipboard::empty() const
{
    return m_widgets.empty() && m_actions.empty();
}
}

QT_END_NAMESPACE
