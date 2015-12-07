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
                if (DomProperty *prop = variantToDomProperty(afb, widget, it.key(), it.value()))
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
