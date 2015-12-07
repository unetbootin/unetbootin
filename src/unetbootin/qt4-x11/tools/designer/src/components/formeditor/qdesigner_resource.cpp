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

#include "qdesigner_resource.h"
#include "formwindow.h"
#include "dynamicpropertysheet.h"
#include "qdesigner_tabwidget_p.h"
#include "qdesigner_toolbox_p.h"
#include "qdesigner_stackedbox_p.h"
#include "qdesigner_toolbar_p.h"
#include "qdesigner_dockwidget_p.h"
#include "qdesigner_menu_p.h"
#include "qdesigner_menubar_p.h"
#include "qdesigner_membersheet_p.h"
#include "qtresourcemodel_p.h"
#include "qmdiarea_container.h"
#include "qwizard_container.h"
#include "itemview_propertysheet.h"
#include "layout_propertysheet.h"

#include <ui4_p.h>
#include <formbuilderextra_p.h>
#include <resourcebuilder_p.h>
#include <textbuilder_p.h>
#include <qdesigner_widgetitem_p.h>

// shared
#include <widgetdatabase_p.h>
#include <metadatabase_p.h>
#include <layout_p.h>
#include <layoutinfo_p.h>
#include <spacer_widget_p.h>
#include <pluginmanager_p.h>
#include <widgetfactory_p.h>
#include <abstractlanguage.h>
#include <abstractintrospection_p.h>

#include <qlayout_widget_p.h>
#include <qdesigner_utils_p.h>
#include <ui4_p.h>

// sdk
#include <QtDesigner/QDesignerPropertySheetExtension>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerExtraInfoExtension>
#include <QtDesigner/QDesignerFormWindowToolInterface>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerContainerExtension>
#include <abstractdialoggui_p.h>

#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QLayout>
#include <QtGui/QFormLayout>
#include <QtGui/QTabWidget>
#include <QtGui/QToolBox>
#include <QtGui/QStackedWidget>
#include <QtGui/QToolBar>
#include <QtGui/QTabBar>
#include <QtGui/QAction>
#include <QtGui/QActionGroup>
#include <QtGui/QButtonGroup>
#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QtGui/QSplitter>
#include <QtGui/QMdiArea>
#include <QtGui/QWorkspace>
#include <QtGui/QMenuBar>
#include <QtGui/QFileDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QTreeView>
#include <QtGui/QTableView>
#include <QtGui/QWizardPage>
#include <private/qlayoutengine_p.h>

#include <QtCore/QBuffer>
#include <QtCore/QDir>
#include <QtCore/QMetaProperty>
#include <QtCore/qdebug.h>
#include <QtCore/QXmlStreamWriter>

Q_DECLARE_METATYPE(QWidgetList)

QT_BEGIN_NAMESPACE

namespace {
    typedef QList<DomProperty*> DomPropertyList;
}

static const char *currentUiVersion = "4.0";
static const char *clipboardObjectName = "__qt_fake_top_level";

#define OLD_RESOURCE_FORMAT // Support pre 4.4 format.

namespace qdesigner_internal {

// -------------------- QDesignerResourceBuilder: A resource builder that works on the property sheet icon types.
class QDesignerResourceBuilder : public QResourceBuilder
{
public:
    QDesignerResourceBuilder(QDesignerFormEditorInterface *core, DesignerPixmapCache *pixmapCache, DesignerIconCache *iconCache);

    void setPixmapCache(DesignerPixmapCache *pixmapCache) { m_pixmapCache = pixmapCache; }
    void setIconCache(DesignerIconCache *iconCache)       { m_iconCache = iconCache; }
    bool isSaveRelative() const                           { return m_saveRelative; }
    void setSaveRelative(bool relative)                   { m_saveRelative = relative; }
    QStringList usedQrcFiles() const                      { return m_usedQrcFiles.keys(); }
#ifdef OLD_RESOURCE_FORMAT
    QStringList loadedQrcFiles() const                    { return m_loadedQrcFiles.keys(); } // needed only for loading old resource attribute of <iconset> tag.
#endif

    virtual QVariant loadResource(const QDir &workingDirectory, const DomProperty *icon) const;

    virtual QVariant toNativeValue(const QVariant &value) const;

    virtual DomProperty *saveResource(const QDir &workingDirectory, const QVariant &value) const;

    virtual bool isResourceType(const QVariant &value) const;
private:

    QDesignerFormEditorInterface *m_core;
    DesignerPixmapCache  *m_pixmapCache;
    DesignerIconCache    *m_iconCache;
    bool                  m_saveRelative;
    mutable QMap<QString, bool>   m_usedQrcFiles;
    mutable QMap<QString, bool>   m_loadedQrcFiles;
};

QDesignerResourceBuilder::QDesignerResourceBuilder(QDesignerFormEditorInterface *core, DesignerPixmapCache *pixmapCache, DesignerIconCache *iconCache) :
    m_core(core),
    m_pixmapCache(pixmapCache),
    m_iconCache(iconCache),
    m_saveRelative(true)
{
}

static inline void setIconPixmap(QIcon::Mode m, QIcon::State s, const QDir &workingDirectory, const QString &v, PropertySheetIconValue &icon)
{
    icon.setPixmap(m, s, PropertySheetPixmapValue(QFileInfo(workingDirectory, v).absoluteFilePath()));
}

QVariant QDesignerResourceBuilder::loadResource(const QDir &workingDirectory, const DomProperty *property) const
{
    switch (property->kind()) {
        case DomProperty::Pixmap: {
            PropertySheetPixmapValue pixmap;
            DomResourcePixmap *dp = property->elementPixmap();
            if (!dp->text().isEmpty()) {
                pixmap.setPath(QFileInfo(workingDirectory, dp->text()).absoluteFilePath());
#ifdef OLD_RESOURCE_FORMAT
                if (dp->hasAttributeResource())
                    m_loadedQrcFiles.insert(QFileInfo(workingDirectory, dp->attributeResource()).absoluteFilePath(), false);
#endif
            }
            return qVariantFromValue(pixmap);
        }

        case DomProperty::IconSet: {
            PropertySheetIconValue icon;
            DomResourceIcon *di = property->elementIconSet();
            if (const int flags = iconStateFlags(di)) { // new, post 4.4 format
                if (flags & NormalOff)
                    setIconPixmap(QIcon::Normal, QIcon::Off, workingDirectory, di->elementNormalOff()->text(), icon);
                if (flags & NormalOn)
                    setIconPixmap(QIcon::Normal, QIcon::On, workingDirectory, di->elementNormalOn()->text(), icon);
                if (flags & DisabledOff)
                    setIconPixmap(QIcon::Disabled, QIcon::Off, workingDirectory, di->elementDisabledOff()->text(), icon);
                if (flags & DisabledOn)
                    setIconPixmap(QIcon::Disabled, QIcon::On, workingDirectory, di->elementDisabledOn()->text(), icon);
                if (flags & ActiveOff)
                    setIconPixmap(QIcon::Active, QIcon::Off, workingDirectory, di->elementActiveOff()->text(), icon);
                if (flags & ActiveOn)
                    setIconPixmap(QIcon::Active, QIcon::On, workingDirectory, di->elementActiveOn()->text(), icon);
                if (flags & SelectedOff)
                    setIconPixmap(QIcon::Selected, QIcon::Off, workingDirectory, di->elementSelectedOff()->text(), icon);
                if (flags & SelectedOn)
                    setIconPixmap(QIcon::Selected, QIcon::On, workingDirectory, di->elementSelectedOn()->text(), icon);
            } else {
#ifdef OLD_RESOURCE_FORMAT
                setIconPixmap(QIcon::Normal, QIcon::Off, workingDirectory, di->text(), icon);
                if (di->hasAttributeResource())
                    m_loadedQrcFiles.insert(QFileInfo(workingDirectory, di->attributeResource()).absoluteFilePath(), false);
#endif
            }
            return qVariantFromValue(icon);
        }
        default:
            break;
    }
    return QVariant();
}

QVariant QDesignerResourceBuilder::toNativeValue(const QVariant &value) const
{
    if (qVariantCanConvert<PropertySheetPixmapValue>(value)) {
        if (m_pixmapCache)
            return m_pixmapCache->pixmap(qVariantValue<PropertySheetPixmapValue>(value));
    } else if (qVariantCanConvert<PropertySheetIconValue>(value)) {
        if (m_iconCache)
            return m_iconCache->icon(qVariantValue<PropertySheetIconValue>(value));
    }
    return value;
}

DomProperty *QDesignerResourceBuilder::saveResource(const QDir &workingDirectory, const QVariant &value) const
{
    DomProperty *p = new DomProperty;
    if (qVariantCanConvert<PropertySheetPixmapValue>(value)) {
        const PropertySheetPixmapValue pix = qvariant_cast<PropertySheetPixmapValue>(value);
        DomResourcePixmap *rp = new DomResourcePixmap;
        const QString pixPath = pix.path();
        switch (pix.pixmapSource(m_core)) {
        case PropertySheetPixmapValue::LanguageResourcePixmap:
            rp->setText(pixPath);
            break;
        case PropertySheetPixmapValue::ResourcePixmap: {
            rp->setText(pixPath);
            const QString qrcFile = m_core->resourceModel()->qrcPath(pixPath);
            if (!qrcFile.isEmpty()) {
                m_usedQrcFiles.insert(qrcFile, false);
#ifdef OLD_RESOURCE_FORMAT  // Legacy: Add qrc path
                rp->setAttributeResource(workingDirectory.relativeFilePath(qrcFile));
#endif
            }
        }
            break;
        case PropertySheetPixmapValue::FilePixmap:
            rp->setText(m_saveRelative ? workingDirectory.relativeFilePath(pixPath) : pixPath);
            break;
        }
        p->setElementPixmap(rp);
        return p;
    } else if (qVariantCanConvert<PropertySheetIconValue>(value)) {
        const PropertySheetIconValue icon = qvariant_cast<PropertySheetIconValue>(value);
        const QMap<QPair<QIcon::Mode, QIcon::State>, PropertySheetPixmapValue> pixmaps = icon.paths();
        if (!pixmaps.isEmpty()) {
            DomResourceIcon *ri = new DomResourceIcon;
            QMapIterator<QPair<QIcon::Mode, QIcon::State>, PropertySheetPixmapValue> itPix(pixmaps);
            while (itPix.hasNext()) {
                const QIcon::Mode mode = itPix.next().key().first;
                const QIcon::State state = itPix.key().second;
                DomResourcePixmap *rp = new DomResourcePixmap;
                const PropertySheetPixmapValue pix = itPix.value();
                const PropertySheetPixmapValue::PixmapSource ps = pix.pixmapSource(m_core);
                const QString pixPath = pix.path();
                rp->setText(ps == PropertySheetPixmapValue::FilePixmap && m_saveRelative ? workingDirectory.relativeFilePath(pixPath) : pixPath);
                if (state == QIcon::Off) {
                    switch (mode) {
                        case QIcon::Normal:
                            ri->setElementNormalOff(rp);
#ifdef OLD_RESOURCE_FORMAT  // Legacy: Set Normal off as text/path in old format.
                            ri->setText(rp->text());
#endif
                            if (ps == PropertySheetPixmapValue::ResourcePixmap) {
                                // Be sure that ri->text() file comes from active resourceSet (i.e. make appropriate
                                // resourceSet active before calling this method).
                                const QString qrcFile = m_core->resourceModel()->qrcPath(ri->text());
                                if (!qrcFile.isEmpty()) {
                                    m_usedQrcFiles.insert(qrcFile, false);
#ifdef OLD_RESOURCE_FORMAT  // Legacy: Set Normal off as text/path in old format.
                                    ri->setAttributeResource(workingDirectory.relativeFilePath(qrcFile));
#endif
                                }
                            }
                        break;
                        case QIcon::Disabled: ri->setElementDisabledOff(rp); break;
                        case QIcon::Active:   ri->setElementActiveOff(rp);   break;
                        case QIcon::Selected: ri->setElementSelectedOff(rp); break;
                    }
                } else {
                    switch (mode) {
                        case QIcon::Normal:   ri->setElementNormalOn(rp);   break;
                        case QIcon::Disabled: ri->setElementDisabledOn(rp); break;
                        case QIcon::Active:   ri->setElementActiveOn(rp);   break;
                        case QIcon::Selected: ri->setElementSelectedOn(rp); break;
                    }
                }
            }
            p->setElementIconSet(ri);
            return p;
        }
    }
    delete p;
    return 0;
}

bool QDesignerResourceBuilder::isResourceType(const QVariant &value) const
{
    if (qVariantCanConvert<PropertySheetPixmapValue>(value) || qVariantCanConvert<PropertySheetIconValue>(value))
        return true;
    return false;
}
// ------------------------- QDesignerTextBuilder
class QDesignerTextBuilder : public QTextBuilder
{
public:
    QDesignerTextBuilder() {}

    virtual QVariant loadText(const DomProperty *icon) const;

    virtual QVariant toNativeValue(const QVariant &value) const;

    virtual DomProperty *saveText(const QVariant &value) const;
};

QVariant QDesignerTextBuilder::loadText(const DomProperty *text) const
{
    const DomString *str = text->elementString();
    PropertySheetStringValue strVal(str->text());
    if (str->hasAttributeComment()) {
        strVal.setDisambiguation(str->attributeComment());
    }
    if (str->hasAttributeExtraComment()) {
        strVal.setComment(str->attributeExtraComment());
    }
    if (str->hasAttributeNotr()) {
        const QString notr = str->attributeNotr();
        const bool translatable = !(notr == QLatin1String("true") || notr == QLatin1String("yes"));
        if (!translatable)
            strVal.setTranslatable(translatable);
    }
    return qVariantFromValue(strVal);
}

QVariant QDesignerTextBuilder::toNativeValue(const QVariant &value) const
{
    if (qVariantCanConvert<PropertySheetStringValue>(value))
        return qVariantFromValue(qVariantValue<PropertySheetStringValue>(value).value());
    return value;
}

DomProperty *QDesignerTextBuilder::saveText(const QVariant &value) const
{
    if (!qVariantCanConvert<PropertySheetStringValue>(value) && !qVariantCanConvert<QString>(value))
        return 0;

    DomProperty *property = new DomProperty();
    DomString *domStr = new DomString();

    if (qVariantCanConvert<PropertySheetStringValue>(value)) {
        PropertySheetStringValue str = qVariantValue<PropertySheetStringValue>(value);

        domStr->setText(str.value());

        const QString property_comment = str.disambiguation();
        if (!property_comment.isEmpty())
            domStr->setAttributeComment(property_comment);
        const QString property_extraComment = str.comment();
        if (!property_extraComment.isEmpty())
            domStr->setAttributeExtraComment(property_extraComment);
        const bool property_translatable = str.translatable();
        if (!property_translatable)
            domStr->setAttributeNotr(QLatin1String("true"));
    } else {
        domStr->setText(value.toString());
    }

    property->setElementString(domStr);
    return property;
}

QDesignerResource::QDesignerResource(FormWindow *formWindow)  :
    QEditorFormBuilder(formWindow->core()),
    m_formWindow(formWindow),
    m_topLevelSpacerCount(0),
    m_copyWidget(false),
    m_selected(0),
    m_resourceBuilder(new QDesignerResourceBuilder(m_formWindow->core(), m_formWindow->pixmapCache(), m_formWindow->iconCache()))
{
    setWorkingDirectory(formWindow->absoluteDir());
    setResourceBuilder(m_resourceBuilder);
    setTextBuilder(new QDesignerTextBuilder());

    // ### generalise
    const QString designerWidget = QLatin1String("QDesignerWidget");
    const QString layoutWidget   = QLatin1String("QLayoutWidget");
    const QString widget = QLatin1String("QWidget");
    m_internal_to_qt.insert(layoutWidget, widget);
    m_internal_to_qt.insert(designerWidget, widget);
    m_internal_to_qt.insert(QLatin1String("QDesignerDialog"), QLatin1String("QDialog"));
    m_internal_to_qt.insert(QLatin1String("QDesignerMenuBar"), QLatin1String("QMenuBar"));
    m_internal_to_qt.insert(QLatin1String("QDesignerMenu"), QLatin1String("QMenu"));
    m_internal_to_qt.insert(QLatin1String("QDesignerDockWidget"), QLatin1String("QDockWidget"));
    m_internal_to_qt.insert(QLatin1String("QDesignerQ3WidgetStack"), QLatin1String("Q3WidgetStack"));

    // invert
    QHash<QString, QString>::const_iterator cend = m_internal_to_qt.constEnd();
    for (QHash<QString, QString>::const_iterator it = m_internal_to_qt.constBegin();it != cend; ++it )  {
        if (it.value() != designerWidget  && it.value() != layoutWidget)
            m_qt_to_internal.insert(it.value(), it.key());

    }
}

QDesignerResource::~QDesignerResource()
{
}

static inline QString messageBoxTitle()
{
    return QApplication::translate("Designer", "Qt Designer");
}

void QDesignerResource::save(QIODevice *dev, QWidget *widget)
{
    m_topLevelSpacerCount = 0;

    QAbstractFormBuilder::save(dev, widget);

    if (QSimpleResource::warningsEnabled() && m_topLevelSpacerCount != 0) {
        const QString message = QApplication::translate("Designer", "This file contains top level spacers.<br>"
                           "They have <b>NOT</b> been saved into the form.");
        const QString infoMessage = QApplication::translate("Designer", "Perhaps you forgot to create a layout?");

        core()->dialogGui()->message(widget->window(), QDesignerDialogGuiInterface::TopLevelSpacerMessage,
                                     QMessageBox::Warning, messageBoxTitle(), message, infoMessage,
                                     QMessageBox::Ok);
    }
}

void QDesignerResource::saveDom(DomUI *ui, QWidget *widget)
{
    QAbstractFormBuilder::saveDom(ui, widget);

    QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), widget);
    Q_ASSERT(sheet != 0);

    const QVariant classVar = sheet->property(sheet->indexOf(QLatin1String("objectName")));
    QString classStr;
    if (classVar.canConvert(QVariant::String))
        classStr = classVar.toString();
    else
        classStr = qVariantValue<PropertySheetStringValue>(classVar).value();
    ui->setElementClass(classStr);

    for (int index = 0; index < m_formWindow->toolCount(); ++index) {
        QDesignerFormWindowToolInterface *tool = m_formWindow->tool(index);
        Q_ASSERT(tool != 0);
        tool->saveToDom(ui, widget);
    }

    const QString author = m_formWindow->author();
    if (!author.isEmpty()) {
        ui->setElementAuthor(author);
    }

    const QString comment = m_formWindow->comment();
    if (!comment.isEmpty()) {
        ui->setElementComment(comment);
    }

    const QString exportMacro = m_formWindow->exportMacro();
    if (!exportMacro.isEmpty()) {
        ui->setElementExportMacro(exportMacro);
    }

    const QVariantMap designerFormData = m_formWindow->formData();
    if (!designerFormData.empty()) {
        DomPropertyList domPropertyList;
        const  QVariantMap::const_iterator cend = designerFormData.constEnd();
        for (QVariantMap::const_iterator it = designerFormData.constBegin(); it != cend; ++it) {
            if (DomProperty *prop = variantToDomProperty(this, widget->metaObject(), it.key(), it.value()))
                domPropertyList += prop;
        }
        if (!domPropertyList.empty()) {
            DomDesignerData* domDesignerFormData = new DomDesignerData;
            domDesignerFormData->setElementProperty(domPropertyList);
            ui->setElementDesignerdata(domDesignerFormData);
        }
    }

    if (!m_formWindow->includeHints().isEmpty()) {
        const QString local = QLatin1String("local");
        const QString global = QLatin1String("global");
        QList<DomInclude*> ui_includes;
        foreach (QString includeHint, m_formWindow->includeHints()) {
            if (includeHint.isEmpty())
                continue;
            DomInclude *incl = new DomInclude;
            const QString location = includeHint.at(0) == QLatin1Char('<') ? global : local;
            includeHint.remove(QLatin1Char('"'));
            includeHint.remove(QLatin1Char('<'));
            includeHint.remove(QLatin1Char('>'));
            incl->setAttributeLocation(location);
            incl->setText(includeHint);
            ui_includes.append(incl);
        }

        DomIncludes *includes = new DomIncludes;
        includes->setElementInclude(ui_includes);
        ui->setElementIncludes(includes);
    }

    int defaultMargin = INT_MIN, defaultSpacing = INT_MIN;
    m_formWindow->layoutDefault(&defaultMargin, &defaultSpacing);

    if (defaultMargin != INT_MIN || defaultSpacing != INT_MIN) {
        DomLayoutDefault *def = new DomLayoutDefault;
        if (defaultMargin != INT_MIN)
            def->setAttributeMargin(defaultMargin);
        if (defaultSpacing != INT_MIN)
            def->setAttributeSpacing(defaultSpacing);
        ui->setElementLayoutDefault(def);
    }

    QString marginFunction, spacingFunction;
    m_formWindow->layoutFunction(&marginFunction, &spacingFunction);
    if (!marginFunction.isEmpty() || !spacingFunction.isEmpty()) {
        DomLayoutFunction *def = new DomLayoutFunction;

        if (!marginFunction.isEmpty())
            def->setAttributeMargin(marginFunction);
        if (!spacingFunction.isEmpty())
            def->setAttributeSpacing(spacingFunction);
        ui->setElementLayoutFunction(def);
    }

    QString pixFunction = m_formWindow->pixmapFunction();
    if (!pixFunction.isEmpty()) {
        ui->setElementPixmapFunction(pixFunction);
    }

    if (QDesignerExtraInfoExtension *extra = qt_extension<QDesignerExtraInfoExtension*>(core()->extensionManager(), core()))
        extra->saveUiExtraInfo(ui);

    if (MetaDataBase *metaDataBase = qobject_cast<MetaDataBase *>(core()->metaDataBase())) {
        const MetaDataBaseItem *item = metaDataBase->metaDataBaseItem(m_formWindow->mainContainer());
        const QStringList fakeSlots = item->fakeSlots();
        const QStringList fakeSignals  =item->fakeSignals();
        if (!fakeSlots.empty() || !fakeSignals.empty()) {
            DomSlots *domSlots = new DomSlots();
            domSlots->setElementSlot(fakeSlots);
            domSlots->setElementSignal(fakeSignals);
            ui->setElementSlots(domSlots);
        }
    }
}

namespace {
    enum LoadPreCheck {  LoadPreCheckFailed, LoadPreCheckVersion3, LoadPreCheckVersionMismatch,  LoadPreCheckOk  };
    // Pair of major, minor
    typedef QPair<int, int> UiVersion;
}

static UiVersion uiVersion(const QString &attr)
{
    const QStringList versions = attr.split(QLatin1Char('.'));
    if (versions.empty())
        return UiVersion(-1, -1);

    bool ok = false;
    UiVersion rc(versions.at(0).toInt(&ok), 0);

    if (!ok)
        return UiVersion(-1, -1);

    if (versions.size() > 1) {
        const int minorVersion = versions.at(1).toInt(&ok);
        if (ok)
            rc.second =  minorVersion;
    }
    return rc;
}

// Read version and language attributes of an <UI> element.
static bool readUiAttributes(QIODevice *dev, QString *errorMessage,
                             QString *version,
                             QString *language)
{
    const QString uiElement = QLatin1String("ui");
    const QString versionAttribute = QLatin1String("version");
    const QString languageAttribute = QLatin1String("language");
    QXmlStreamReader reader(dev);
    // Read up to first element
    while (!reader.atEnd()) {
        if (reader.readNext() == QXmlStreamReader::StartElement) {
            const QStringRef tag = reader.name();
            if (reader.name().compare(uiElement, Qt::CaseInsensitive) == 0) {
                const QXmlStreamAttributes attributes = reader.attributes();
                if (attributes.hasAttribute(versionAttribute))
                    *version = attributes.value(versionAttribute).toString();
                if (attributes.hasAttribute(languageAttribute))
                    *language = attributes.value(languageAttribute).toString();
                return true;
            } else {
                *errorMessage = QCoreApplication::translate("Designer", "Invalid ui file: The root element <ui> is missing.");
                return false;

            }
        }
    }
    *errorMessage = QCoreApplication::translate("Designer", "An error has occurred while reading the ui file at line %1, column %2: %3")
                    .arg(reader.lineNumber()).arg(reader.columnNumber()).arg(reader.errorString());
    return false;
}

// While loading a file, check language, version and extra extension
static LoadPreCheck loadPrecheck(QDesignerFormEditorInterface *core,
                                 QIODevice *dev,
                                 QString *errorMessage, QString *versionString)
{
    QString language;
    // Read attributes of <ui> and rewind
    if (!readUiAttributes(dev, errorMessage, versionString, &language)) {
        // XML error: Mimick the behaviour occurring if an XML error is
        // detected later on, report to warning log and have embedding
        // application display a dialog.
        designerWarning(*errorMessage);
        errorMessage->clear();
        return LoadPreCheckFailed;
    }
    dev->seek(0);

    // Check language unless extension present (Jambi)
    if (!language.isEmpty() && !qt_extension<QDesignerLanguageExtension*>(core->extensionManager(), core)) {
        if (language.toLower() != QLatin1String("c++")) {
            // Jambi?!
            *errorMessage = QApplication::translate("Designer", "This file cannot be read because it was created using %1.").arg(language);
            return LoadPreCheckFailed;
        }
    }

    // Version
    if (!versionString->isEmpty()) {
        const UiVersion version = uiVersion(*versionString);
        switch (version.first) {
        case 3:
            return  LoadPreCheckVersion3;
        case 4:
            break;
        default:
            *errorMessage = QApplication::translate("Designer", "This file was created using Designer from Qt-%1 and cannot be read.").arg(*versionString);
            return LoadPreCheckVersionMismatch;
        }
    }
    return LoadPreCheckOk;
}

QWidget *QDesignerResource::load(QIODevice *dev, QWidget *parentWidget)
{
    // Run loadPreCheck for version and language
    QString errorMessage;
    QString version;
    switch (loadPrecheck(core(), dev,  &errorMessage, &version)) {
    case LoadPreCheckFailed:
    case LoadPreCheckVersionMismatch:
        if (!errorMessage.isEmpty())
            core()->dialogGui()->message(parentWidget->window(), QDesignerDialogGuiInterface::FormLoadFailureMessage,
                                         QMessageBox::Warning, messageBoxTitle(), errorMessage, QMessageBox::Ok);
        return 0;
    case LoadPreCheckVersion3: {
        QWidget *w = 0;
        QByteArray ba;
        if (runUIC( m_formWindow->fileName(), UIC_ConvertV3, ba, errorMessage)) {
            QBuffer buffer(&ba);
            buffer.open(QIODevice::ReadOnly);
            w = load(&buffer, parentWidget);
            if (w) {
                // Force the form to pop up a save file dialog
                m_formWindow->setFileName(QString());
            } else {
                errorMessage = QApplication::translate("Designer", "The converted file could not be read.");
            }
        }
        if (w) {
            const QString message = QApplication::translate("Designer",
                                     "This file was created using Designer from Qt-%1 and"
                                     " will be converted to a new form by Qt Designer.").arg(version);
            const QString infoMessage = QApplication::translate("Designer",
                                     "The old form has not been touched, but you will have to save the form"
                                     " under a new name.");

            core()->dialogGui()->message(parentWidget->window(),
                                         QDesignerDialogGuiInterface::UiVersionMismatchMessage,
                                         QMessageBox::Information, messageBoxTitle(), message, infoMessage,
                                         QMessageBox::Ok);
            return w;
        }

        const QString message = QApplication::translate("Designer",
                             "This file was created using Designer from Qt-%1 and "
                             "could not be read:\n%2").arg(version).arg(errorMessage);
        const QString infoMessage = QApplication::translate("Designer",
                             "Please run it through <b>uic3&nbsp;-convert</b> to convert "
                             "it to Qt-4's ui format.");
        core()->dialogGui()->message(parentWidget->window(), QDesignerDialogGuiInterface::FormLoadFailureMessage,
                                     QMessageBox::Warning, messageBoxTitle(), message, infoMessage,
                                     QMessageBox::Ok);
        return 0;
    }

    case LoadPreCheckOk:
        break;
    }
    return QEditorFormBuilder::load(dev, parentWidget);
}

bool QDesignerResource::saveRelative() const
{
    return m_resourceBuilder->isSaveRelative();
}

void QDesignerResource::setSaveRelative(bool relative)
{
    m_resourceBuilder->setSaveRelative(relative);
}

static bool addFakeMethods(const DomSlots *domSlots, QStringList &fakeSlots, QStringList &fakeSignals)
{
    if (!domSlots)
        return false;

    bool rc = false;
    foreach (const QString &fakeSlot, domSlots->elementSlot())
        if (fakeSlots.indexOf(fakeSlot) == -1) {
            fakeSlots += fakeSlot;
            rc = true;
        }

    foreach (const QString &fakeSignal, domSlots->elementSignal())
        if (fakeSignals.indexOf(fakeSignal) == -1) {
            fakeSignals += fakeSignal;
            rc = true;
        }
    return rc;
}

QWidget *QDesignerResource::create(DomUI *ui, QWidget *parentWidget)
{
    // Load extra info extension. This is used by Jambi for preventing
    // C++ ui files from being loaded
    if (QDesignerExtraInfoExtension *extra = qt_extension<QDesignerExtraInfoExtension*>(core()->extensionManager(), core())) {
        if (!extra->loadUiExtraInfo(ui)) {
            const QString errorMessage = QApplication::translate("Designer", "This file cannot be read because the extra info extension failed to load.");
            core()->dialogGui()->message(parentWidget->window(), QDesignerDialogGuiInterface::FormLoadFailureMessage,
                                         QMessageBox::Warning, messageBoxTitle(), errorMessage, QMessageBox::Ok);
            return 0;
        }
    }

    qdesigner_internal::WidgetFactory *factory = qobject_cast<qdesigner_internal::WidgetFactory*>(core()->widgetFactory());
    Q_ASSERT(factory != 0);

    QDesignerFormWindowInterface *previousFormWindow = factory->currentFormWindow(m_formWindow);

    m_isMainWidget = true;
    QDesignerWidgetItemInstaller wii; // Make sure we use QDesignerWidgetItem.
    QWidget *mainWidget = QAbstractFormBuilder::create(ui, parentWidget);

    if (mainWidget && m_formWindow) {
        m_formWindow->setAuthor(ui->elementAuthor());
        m_formWindow->setComment(ui->elementComment());
        m_formWindow->setExportMacro(ui->elementExportMacro());

        // Designer data
        QVariantMap designerFormData;
        if (ui->hasElementDesignerdata()) {
            const DomPropertyList domPropertyList = ui->elementDesignerdata()->elementProperty();
            const DomPropertyList::const_iterator cend = domPropertyList.constEnd();
            for (DomPropertyList::const_iterator it = domPropertyList.constBegin(); it != cend; ++it) {
                const QVariant vprop = domPropertyToVariant(this, mainWidget->metaObject(), *it);
                if (vprop.type() != QVariant::Invalid)
                    designerFormData.insert((*it)->attributeName(), vprop);
            }
        }
        m_formWindow->setFormData(designerFormData);

        m_formWindow->setPixmapFunction(ui->elementPixmapFunction());

        if (DomLayoutDefault *def = ui->elementLayoutDefault()) {
            m_formWindow->setLayoutDefault(def->attributeMargin(), def->attributeSpacing());
        }

        if (DomLayoutFunction *fun = ui->elementLayoutFunction()) {
            m_formWindow->setLayoutFunction(fun->attributeMargin(), fun->attributeSpacing());
        }

        if (DomIncludes *includes = ui->elementIncludes()) {
            const QString global = QLatin1String("global");
            QStringList includeHints;
            foreach (DomInclude *incl, includes->elementInclude()) {
                QString text = incl->text();

                if (text.isEmpty())
                    continue;

                if (incl->hasAttributeLocation() && incl->attributeLocation() == global ) {
                    text = text.prepend(QLatin1Char('<')).append(QLatin1Char('>'));
                } else {
                    text = text.prepend(QLatin1Char('"')).append(QLatin1Char('"'));
                }

                includeHints.append(text);
            }

            m_formWindow->setIncludeHints(includeHints);
        }

        // Register all button groups the form builder adds as children of the main container for them to be found
        // in the signal slot editor
        const QObjectList mchildren = mainWidget->children();
        if (!mchildren.empty()) {
            QDesignerMetaDataBaseInterface *mdb = core()->metaDataBase();
            const QObjectList::const_iterator cend = mchildren.constEnd();
            for (QObjectList::const_iterator it = mchildren.constBegin(); it != cend; ++it)
                if (QButtonGroup *bg = qobject_cast<QButtonGroup*>(*it))
                    mdb->add(bg);
        }
        // Load tools
        for (int index = 0; index < m_formWindow->toolCount(); ++index) {
            QDesignerFormWindowToolInterface *tool = m_formWindow->tool(index);
            Q_ASSERT(tool != 0);
            tool->loadFromDom(ui, mainWidget);
        }
    }

    factory->currentFormWindow(previousFormWindow);

    if (const DomSlots *domSlots = ui->elementSlots()) {
        if (MetaDataBase *metaDataBase = qobject_cast<MetaDataBase *>(core()->metaDataBase())) {
            QStringList fakeSlots;
            QStringList fakeSignals;
            if (addFakeMethods(domSlots, fakeSlots, fakeSignals)) {
                MetaDataBaseItem *item = metaDataBase->metaDataBaseItem(mainWidget);
                item->setFakeSlots(fakeSlots);
                item->setFakeSignals(fakeSignals);
            }
        }
    }
    if (mainWidget) {
        // Initialize the mainwindow geometry. Has it been  explicitly specified?
        bool hasExplicitGeometry = false;
        const QList<DomProperty *> properties = ui->elementWidget()->elementProperty();
        if (!properties.empty()) {
            const QString geometry = QLatin1String("geometry");
            foreach (const DomProperty *p, properties)
                if (p->attributeName() == geometry) {
                    hasExplicitGeometry = true;
                    break;
                }
        }
        if (hasExplicitGeometry) {
            // Geometry was specified explicitly: Verify that smartMinSize is respected
            // (changed fonts, label wrapping policies, etc). This does not happen automatically in docked mode.
            const QSize size = mainWidget->size();
            const QSize minSize = size.expandedTo(qSmartMinSize(mainWidget));
            if (minSize != size)
                mainWidget->resize(minSize);
        } else {
            // No explicit Geometry: perform an adjustSize() to resize the form correctly before embedding it into a container
            // (which might otherwise squeeze the form)
            mainWidget->adjustSize();
        }
        // Some integration wizards create forms with main containers
        // based on derived classes of QWidget and load them into Designer
        // without the plugin existing. This will trigger the auto-promotion
        // mechanism of Designer, which will set container=false for
        // QWidgets. For the main container, force container=true and warn.
        const QDesignerWidgetDataBaseInterface *wdb = core()->widgetDataBase();
        const int wdbIndex = wdb->indexOfObject(mainWidget);
        if (wdbIndex != -1) {
            QDesignerWidgetDataBaseItemInterface *item = wdb->item(wdbIndex);
            // Promoted main container that is not of container type
            if (item->isPromoted() && !item->isContainer()) {
                item->setContainer(true);
                qWarning("** WARNING The form's main container is an unknown custom widget '%s'."
                         " Defaulting to a promoted instance of '%s', assuming container.",
                         item->name().toUtf8().constData(), item->extends().toUtf8().constData());
            }
        }
    }
    return mainWidget;
}

QWidget *QDesignerResource::create(DomWidget *ui_widget, QWidget *parentWidget)
{
    const QString className = ui_widget->attributeClass();
    if (!m_isMainWidget && className == QLatin1String("QWidget") && ui_widget->elementLayout().size() &&
                !ui_widget->hasAttributeNative()) {
        // ### check if elementLayout.size() == 1

        QDesignerContainerExtension *container = qt_extension<QDesignerContainerExtension*>(core()->extensionManager(), parentWidget);

        if (container == 0) {
            // generate a QLayoutWidget iff the parent is not an QDesignerContainerExtension.
            ui_widget->setAttributeClass(QLatin1String("QLayoutWidget"));
        }
    }

    // save the actions
    const QList<DomActionRef*> actionRefs = ui_widget->elementAddAction();
    ui_widget->setElementAddAction(QList<DomActionRef*>());

    QWidget *w = QAbstractFormBuilder::create(ui_widget, parentWidget);

    // restore the actions
    ui_widget->setElementAddAction(actionRefs);

    if (w == 0)
       return 0;

    // ### generalize using the extension manager
    QDesignerMenu *menu = qobject_cast<QDesignerMenu*>(w);
    QDesignerMenuBar *menuBar = qobject_cast<QDesignerMenuBar*>(w);

    if (menu) {
        menu->interactive(false);
        menu->hide();
    } else if (menuBar) {
        menuBar->interactive(false);
    }

    foreach (DomActionRef *ui_action_ref, actionRefs) {
        const QString name = ui_action_ref->attributeName();
        if (name == QLatin1String("separator")) {
            QAction *sep = new QAction(w);
            sep->setSeparator(true);
            w->addAction(sep);
            addMenuAction(sep);
        } else if (QAction *a = m_actions.value(name)) {
            w->addAction(a);
        } else if (QActionGroup *g = m_actionGroups.value(name)) {
            w->addActions(g->actions());
        } else if (QMenu *menu = qFindChild<QMenu*>(w, name)) {
            w->addAction(menu->menuAction());
            addMenuAction(menu->menuAction());
        }
    }

    if (menu) {
        menu->interactive(true);
        menu->adjustSpecialActions();
    } else if (menuBar) {
        menuBar->interactive(true);
        menuBar->adjustSpecialActions();
    }

    ui_widget->setAttributeClass(className); // fix the class name
    applyExtensionDataFromDOM(this, core(), ui_widget, w, true);

    // store user-defined scripts
    if (MetaDataBase *metaDataBase = qobject_cast<MetaDataBase *>(core()->metaDataBase())) {
        const QString designerSource = QLatin1String("designer");
        const DomScripts domScripts = ui_widget->elementScript();
        if (!domScripts.empty()) {
            foreach (const DomScript *script, domScripts) {
                if (script->hasAttributeSource() && script->attributeSource() == designerSource) {
                    metaDataBase->metaDataBaseItem(w)->setScript(script->text());
                }
            }
        }
    }

    return w;
}

QLayout *QDesignerResource::create(DomLayout *ui_layout, QLayout *layout, QWidget *parentWidget)
{
    QLayout *l = QAbstractFormBuilder::create(ui_layout, layout, parentWidget);

    if (QGridLayout *gridLayout = qobject_cast<QGridLayout*>(l)) {
        QLayoutSupport::createEmptyCells(gridLayout);
    } else {
        if (QFormLayout *formLayout = qobject_cast<QFormLayout*>(l))
            QLayoutSupport::createEmptyCells(formLayout);
    }
    // While the actual values are applied by the form builder, we still need
    // to mark them as 'changed'.
    LayoutPropertySheet::markChangedStretchProperties(core(), l, ui_layout);
    return l;
}

QLayoutItem *QDesignerResource::create(DomLayoutItem *ui_layoutItem, QLayout *layout, QWidget *parentWidget)
{
    if (ui_layoutItem->kind() == DomLayoutItem::Spacer) {
        const DomSpacer *domSpacer = ui_layoutItem->elementSpacer();
        const QHash<QString, DomProperty*> properties = propertyMap(domSpacer->elementProperty());
        Spacer *spacer = static_cast<Spacer*>(core()->widgetFactory()->createWidget(QLatin1String("Spacer"), parentWidget));
        if (domSpacer->hasAttributeName())
            changeObjectName(spacer, domSpacer->attributeName());
        core()->metaDataBase()->add(spacer);

        spacer->setInteractiveMode(false);
        applyProperties(spacer, ui_layoutItem->elementSpacer()->elementProperty());
        spacer->setInteractiveMode(true);

        if (m_formWindow) {
            m_formWindow->manageWidget(spacer);
            if (QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), spacer))
                sheet->setChanged(sheet->indexOf(QLatin1String("orientation")), true);
        }

        return new QWidgetItem(spacer);
    } else if (ui_layoutItem->kind() == DomLayoutItem::Layout && parentWidget) {
        DomLayout *ui_layout = ui_layoutItem->elementLayout();
        QLayoutWidget *layoutWidget = new QLayoutWidget(m_formWindow, parentWidget);
        core()->metaDataBase()->add(layoutWidget);
        if (m_formWindow)
            m_formWindow->manageWidget(layoutWidget);
        (void) create(ui_layout, 0, layoutWidget);
        return new QWidgetItem(layoutWidget);
    }
    return QAbstractFormBuilder::create(ui_layoutItem, layout, parentWidget);
}

void QDesignerResource::changeObjectName(QObject *o, QString objName)
{
    m_formWindow->unify(o, objName, true);
    o->setObjectName(objName);

}

/* If the property is a enum or flag value, retrieve
 * the existing enum/flag via property sheet and use it to convert */

static bool readDomEnumerationValue(const DomProperty *p,
                                    const QDesignerPropertySheetExtension* sheet, int index,
                                    QVariant &v)
{
    switch (p->kind()) {
    case DomProperty::Set: {
        const QVariant sheetValue = sheet->property(index);
        if (qVariantCanConvert<PropertySheetFlagValue>(sheetValue)) {
            const PropertySheetFlagValue f = qvariant_cast<PropertySheetFlagValue>(sheetValue);
            bool ok = false;
            v = f.metaFlags.parseFlags(p->elementSet(), &ok);
            if (!ok)
                designerWarning(f.metaFlags.messageParseFailed(p->elementSet()));
            return true;
        }
    }
        break;
    case DomProperty::Enum: {
        const QVariant sheetValue = sheet->property(index);
        if (qVariantCanConvert<PropertySheetEnumValue>(sheetValue)) {
            const PropertySheetEnumValue e = qvariant_cast<PropertySheetEnumValue>(sheetValue);
            bool ok = false;
            v = e.metaEnum.parseEnum(p->elementEnum(), &ok);
            if (!ok)
                designerWarning(e.metaEnum.messageParseFailed(p->elementEnum()));
            return true;
        }
    }
        break;
    default:
        break;
    }
    return false;
}

void QDesignerResource::applyProperties(QObject *o, const QList<DomProperty*> &properties)
{
    if (properties.empty())
        return;

    QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), o);
    if (!sheet)
        return;

    QFormBuilderExtra *formBuilderExtra = QFormBuilderExtra::instance(this);
    QDesignerDynamicPropertySheetExtension *dynamicSheet = qt_extension<QDesignerDynamicPropertySheetExtension*>(core()->extensionManager(), o);
    const bool dynamicPropertiesAllowed = dynamicSheet && dynamicSheet->dynamicPropertiesAllowed();

    const QString objectNameProperty = QLatin1String("objectName");
    const DomPropertyList::const_iterator cend = properties.constEnd();
    for (DomPropertyList::const_iterator it = properties.constBegin(); it != cend; ++it) {
        const DomProperty *p = *it;
        const QString propertyName = p->attributeName();
        const int index = sheet->indexOf(propertyName);
        QVariant v;
        if (!readDomEnumerationValue(p, sheet, index, v))
            v = toVariant(o->metaObject(), *it);

        if (p->kind() == DomProperty::String) {
            if (index != -1 && sheet->property(index).userType() == qMetaTypeId<PropertySheetKeySequenceValue>()) {
                const DomString *key = p->elementString();
                PropertySheetKeySequenceValue keyVal(QKeySequence(key->text()));
                if (key->hasAttributeComment())
                    keyVal.setDisambiguation(key->attributeComment());
                if (key->hasAttributeExtraComment())
                    keyVal.setComment(key->attributeExtraComment());
                if (key->hasAttributeNotr()) {
                    const QString notr = key->attributeNotr();
                    const bool translatable = !(notr == QLatin1String("true") || notr == QLatin1String("yes"));
                    if (!translatable)
                        keyVal.setTranslatable(translatable);
                }
                v = qVariantFromValue(keyVal);
            } else {
                const DomString *str = p->elementString();
                PropertySheetStringValue strVal(v.toString());
                if (str->hasAttributeComment())
                    strVal.setDisambiguation(str->attributeComment());
                if (str->hasAttributeExtraComment())
                    strVal.setComment(str->attributeExtraComment());
                if (str->hasAttributeNotr()) {
                    const QString notr = str->attributeNotr();
                    const bool translatable = !(notr == QLatin1String("true") || notr == QLatin1String("yes"));
                    if (!translatable)
                        strVal.setTranslatable(translatable);
                }
                v = qVariantFromValue(strVal);
            }
        }

        formBuilderExtra->applyPropertyInternally(o, propertyName, v);
        if (index != -1) {
            sheet->setProperty(index, v);
            sheet->setChanged(index, true);
        } else if (dynamicPropertiesAllowed) {
            QVariant defaultValue = QVariant(v.type());
            bool isDefault = (v == defaultValue);
            if (qVariantCanConvert<PropertySheetIconValue>(v)) {
                defaultValue = QVariant(QVariant::Icon);
                isDefault = (qVariantValue<PropertySheetIconValue>(v) == PropertySheetIconValue());
            } else if (qVariantCanConvert<PropertySheetPixmapValue>(v)) {
                defaultValue = QVariant(QVariant::Pixmap);
                isDefault = (qVariantValue<PropertySheetPixmapValue>(v) == PropertySheetPixmapValue());
            } else if (qVariantCanConvert<PropertySheetStringValue>(v)) {
                defaultValue = QVariant(QVariant::String);
                isDefault = (qVariantValue<PropertySheetStringValue>(v) == PropertySheetStringValue());
            } else if (qVariantCanConvert<PropertySheetKeySequenceValue>(v)) {
                defaultValue = QVariant(QVariant::KeySequence);
                isDefault = (qVariantValue<PropertySheetKeySequenceValue>(v) == PropertySheetKeySequenceValue());
            }
            if (defaultValue.type() != QVariant::UserType) {
                const int idx = dynamicSheet->addDynamicProperty(p->attributeName(), defaultValue);
                if (idx != -1) {
                    sheet->setProperty(idx, v);
                    sheet->setChanged(idx, !isDefault);
                }
            }
        }

        if (propertyName == objectNameProperty)
            changeObjectName(o, o->objectName());
    }
}

QWidget *QDesignerResource::createWidget(const QString &widgetName, QWidget *parentWidget, const QString &_name)
{
    QString name = _name;
    QString className = widgetName;
    if (m_isMainWidget)
        m_isMainWidget = false;

    QWidget *w = core()->widgetFactory()->createWidget(className, parentWidget);
    if (!w)
        return 0;

    if (name.isEmpty()) {
        QDesignerWidgetDataBaseInterface *db = core()->widgetDataBase();
        if (QDesignerWidgetDataBaseItemInterface *item = db->item(db->indexOfObject(w)))
            name = qtify(item->name());
    }

    changeObjectName(w, name);

    QDesignerContainerExtension *container = qt_extension<QDesignerContainerExtension*>(core()->extensionManager(), parentWidget);
    if (!qobject_cast<QMenu*>(w) && (!parentWidget || !container)) {
        m_formWindow->manageWidget(w);
        if (parentWidget) {
            QList<QWidget *> list = qVariantValue<QWidgetList>(parentWidget->property("_q_widgetOrder"));
            list.append(w);
            parentWidget->setProperty("_q_widgetOrder", qVariantFromValue(list));
            QList<QWidget *> zOrder = qVariantValue<QWidgetList>(parentWidget->property("_q_zOrder"));
            zOrder.append(w);
            parentWidget->setProperty("_q_zOrder", qVariantFromValue(list));
        }
    } else {
        core()->metaDataBase()->add(w);
    }

    w->setWindowFlags(w->windowFlags() & ~Qt::Window);
    // Make sure it is non-modal (for example, KDialog calls setModal(true) in the constructor).
    w->setWindowModality(Qt::NonModal);

    return w;
}

QLayout *QDesignerResource::createLayout(const QString &layoutName, QObject *parent, const QString &name)
{
    QWidget *layoutBase = 0;
    QLayout *layout = qobject_cast<QLayout*>(parent);

    if (parent->isWidgetType())
        layoutBase = static_cast<QWidget*>(parent);
    else {
        Q_ASSERT( layout != 0 );
        layoutBase = layout->parentWidget();
    }

    LayoutInfo::Type layoutType = LayoutInfo::layoutType(layoutName);
    if (layoutType == LayoutInfo::NoLayout) {
        designerWarning(QCoreApplication::translate("QDesignerResource", "The layout type '%1' is not supported, defaulting to grid.").arg(layoutName));
        layoutType = LayoutInfo::Grid;
    }
    QLayout *lay = core()->widgetFactory()->createLayout(layoutBase, layout, layoutType);
    if (lay != 0)
        changeObjectName(lay, name);

    return lay;
}

// save
DomWidget *QDesignerResource::createDom(QWidget *widget, DomWidget *ui_parentWidget, bool recursive)
{
    QDesignerMetaDataBaseItemInterface *item = core()->metaDataBase()->item(widget);
    if (!item)
        return 0;

    if (qobject_cast<Spacer*>(widget) && m_copyWidget == false) {
        ++m_topLevelSpacerCount;
        return 0;
    }

    QDesignerWidgetDataBaseItemInterface *widgetInfo =  0;
    const int widgetInfoIndex = core()->widgetDataBase()->indexOfObject(widget, false);
    if (widgetInfoIndex != -1) {
        widgetInfo = core()->widgetDataBase()->item(widgetInfoIndex);
        if (widgetInfo->isCustom())
            m_usedCustomWidgets.insert(widgetInfo, true);
    }

    DomWidget *w = 0;

    if (QTabWidget *tabWidget = qobject_cast<QTabWidget*>(widget))
        w = saveWidget(tabWidget, ui_parentWidget);
    else if (QStackedWidget *stackedWidget = qobject_cast<QStackedWidget*>(widget))
        w = saveWidget(stackedWidget, ui_parentWidget);
    else if (QToolBox *toolBox = qobject_cast<QToolBox*>(widget))
        w = saveWidget(toolBox, ui_parentWidget);
    else if (QToolBar *toolBar = qobject_cast<QToolBar*>(widget))
        w = saveWidget(toolBar, ui_parentWidget);
    else if (QDesignerDockWidget *dockWidget = qobject_cast<QDesignerDockWidget*>(widget))
        w = saveWidget(dockWidget, ui_parentWidget);
    else if (QDesignerContainerExtension *container = qt_extension<QDesignerContainerExtension*>(core()->extensionManager(), widget))
        w = saveWidget(widget, container, ui_parentWidget);
    else if (QTreeView *treeView = qobject_cast<QTreeView*>(widget))
        w = saveWidget(treeView, ui_parentWidget);
    else if (QTableView *tableView = qobject_cast<QTableView*>(widget))
        w = saveWidget(tableView, ui_parentWidget);
    else if (QWizardPage *wizardPage = qobject_cast<QWizardPage*>(widget))
        w = saveWidget(wizardPage, ui_parentWidget);
    else
        w = QAbstractFormBuilder::createDom(widget, ui_parentWidget, recursive);

    Q_ASSERT( w != 0 );

    if (!qobject_cast<QLayoutWidget*>(widget) && w->attributeClass() == QLatin1String("QWidget")) {
        w->setAttributeNative(true);
    }

    const QString className = w->attributeClass();
    if (m_internal_to_qt.contains(className))
        w->setAttributeClass(m_internal_to_qt.value(className));

    w->setAttributeName(widget->objectName());

    if (isPromoted( core(), widget)) { // is promoted?
        Q_ASSERT(widgetInfo != 0);

        w->setAttributeName(widget->objectName());
        w->setAttributeClass(widgetInfo->name());

        QList<DomProperty*> prop_list = w->elementProperty();
        foreach (DomProperty *prop, prop_list) {
            if (prop->attributeName() == QLatin1String("geometry")) {
                if (DomRect *rect = prop->elementRect()) {
                    rect->setElementX(widget->x());
                    rect->setElementY(widget->y());
                }
                break;
            }
        }
    } else if (widgetInfo != 0 && m_usedCustomWidgets.contains(widgetInfo)) {
        if (widgetInfo->name() != w->attributeClass())
            w->setAttributeClass(widgetInfo->name());
    }
    addExtensionDataToDOM(this, core(), w, widget);

    addUserDefinedScripts(widget, w);
    return w;
}

DomLayout *QDesignerResource::createDom(QLayout *layout, DomLayout *ui_parentLayout, DomWidget *ui_parentWidget)
{
    QDesignerMetaDataBaseItemInterface *item = core()->metaDataBase()->item(layout);

    if (item == 0) {
        layout = qFindChild<QLayout*>(layout);
        // refresh the meta database item
        item = core()->metaDataBase()->item(layout);
    }

    if (item == 0) {
        // nothing to do.
        return 0;
    }

    if (qobject_cast<QSplitter*>(layout->parentWidget()) != 0) {
        // nothing to do.
        return 0;
    }

    m_chain.push(layout);

    DomLayout *l = QAbstractFormBuilder::createDom(layout, ui_parentLayout, ui_parentWidget);
    Q_ASSERT(l != 0);
    LayoutPropertySheet::stretchAttributesToDom(core(), layout, l);

    m_chain.pop();

    return l;
}

DomLayoutItem *QDesignerResource::createDom(QLayoutItem *item, DomLayout *ui_layout, DomWidget *ui_parentWidget)
{
    DomLayoutItem *ui_item = 0;

    if (Spacer *s = qobject_cast<Spacer*>(item->widget())) {
        if (!core()->metaDataBase()->item(s))
            return 0;

        DomSpacer *spacer = new DomSpacer();
        const QString objectName = s->objectName();
        if (!objectName.isEmpty())
            spacer->setAttributeName(objectName);
        const QList<DomProperty*> properties = computeProperties(item->widget());
        // ### filter the properties
        spacer->setElementProperty(properties);

        ui_item = new DomLayoutItem();
        ui_item->setElementSpacer(spacer);
        m_laidout.insert(item->widget(), true);
    } else if (QLayoutWidget *layoutWidget = qobject_cast<QLayoutWidget*>(item->widget())) {
        // Do not save a QLayoutWidget if it is within a layout (else it is saved as "QWidget"
        Q_ASSERT(layoutWidget->layout());
        DomLayout *l = createDom(layoutWidget->layout(), ui_layout, ui_parentWidget);
        ui_item = new DomLayoutItem();
        ui_item->setElementLayout(l);
        m_laidout.insert(item->widget(), true);
    } else if (!item->spacerItem()) { // we use spacer as fake item in the Designer
        ui_item = QAbstractFormBuilder::createDom(item, ui_layout, ui_parentWidget);
    } else {
        return 0;
    }

    if (m_chain.size() && item->widget()) {
        if (QGridLayout *grid = qobject_cast<QGridLayout*>(m_chain.top())) {
            const int index = Utils::indexOfWidget(grid, item->widget());

            int row, column, rowspan, colspan;
            grid->getItemPosition(index, &row, &column, &rowspan, &colspan);
            ui_item->setAttributeRow(row);
            ui_item->setAttributeColumn(column);

            if (colspan != 1)
                ui_item->setAttributeColSpan(colspan);

            if (rowspan != 1)
                ui_item->setAttributeRowSpan(rowspan);
        } else {
            if (QFormLayout *form = qobject_cast<QFormLayout*>(m_chain.top())) {
                const int index = Utils::indexOfWidget(form, item->widget());
                int row, column, colspan;
                getFormLayoutItemPosition(form, index, &row, &column, 0, &colspan);
                ui_item->setAttributeRow(row);
                ui_item->setAttributeColumn(column);
                if (colspan != 1)
                    ui_item->setAttributeColSpan(colspan);
            }
        }
    }

    return ui_item;
}

static void addFakeMethodsToWidgetDataBase(const DomCustomWidget *domCustomWidget, WidgetDataBaseItem *item)
{
    const DomSlots *domSlots = domCustomWidget->elementSlots();
    if (!domSlots)
        return;

    // Merge in new slots, signals
    QStringList fakeSlots = item->fakeSlots();
    QStringList fakeSignals = item->fakeSignals();
    if (addFakeMethods(domSlots, fakeSlots, fakeSignals)) {
        item->setFakeSlots(fakeSlots);
        item->setFakeSignals(fakeSignals);
    }
}

void QDesignerResource::addCustomWidgetsToWidgetDatabase(DomCustomWidgetList& custom_widget_list)
{
    // Perform one iteration of adding the custom widgets to the database,
    // looking up the base class and inheriting its data.
    // Remove the succeeded custom widgets from the list.
    // Classes whose base class could not be found are left in the list.
    QDesignerWidgetDataBaseInterface *db = m_formWindow->core()->widgetDataBase();
    for (int i=0; i < custom_widget_list.size(); ) {
        bool classInserted = false;
        DomCustomWidget *custom_widget = custom_widget_list[i];
        const QString customClassName = custom_widget->elementClass();
        const QString base_class = custom_widget->elementExtends();
        QString includeFile;
        IncludeType includeType = IncludeLocal;
        if (const DomHeader *header = custom_widget->elementHeader()) {
            includeFile = header->text();
            if (header->hasAttributeLocation() && header->attributeLocation() == QLatin1String("global"))
                includeType = IncludeGlobal;
        }
        const bool domIsContainer = custom_widget->elementContainer();
        // Append a new item
        if (base_class.isEmpty()) {
            WidgetDataBaseItem *item = new WidgetDataBaseItem(customClassName);
            item->setPromoted(false);
            item->setGroup(QApplication::translate("Designer", "Custom Widgets"));
            item->setIncludeFile(buildIncludeFile(includeFile, includeType));
            item->setContainer(domIsContainer);
            item->setCustom(true);
            addFakeMethodsToWidgetDataBase(custom_widget, item);
            db->append(item);
            custom_widget_list.removeAt(i);
            classInserted = true;
        } else {
            // Create a new entry cloned from base class. Note that this will ignore existing
            // classes, eg, plugin custom widgets.
            QDesignerWidgetDataBaseItemInterface *item =
                appendDerived(db, customClassName, QApplication::translate("Designer", "Promoted Widgets"),
                              base_class,
                              buildIncludeFile(includeFile, includeType),
                              true,true);
            // Ok, base class found.
            if (item) {
                // Hack to accommodate for old UI-files in which "contains" is not set properly:
                // Apply "contains" from DOM only if true (else, eg classes from QFrame might not accept
                // dropping child widgets on them as container=false). This also allows for
                // QWidget-derived stacked pages.
                if (domIsContainer)
                    item->setContainer(domIsContainer);

                addFakeMethodsToWidgetDataBase(custom_widget, static_cast<WidgetDataBaseItem*>(item));
                custom_widget_list.removeAt(i);
                classInserted = true;
            }
        }
        // Skip failed item.
        if (!classInserted)
            i++;
    }

}
void QDesignerResource::createCustomWidgets(DomCustomWidgets *dom_custom_widgets)
{
    if (dom_custom_widgets == 0)
        return;
    DomCustomWidgetList custom_widget_list = dom_custom_widgets->elementCustomWidget();
    // Attempt to insert each item derived from its base class.
    // This should at most require two iterations in the event that the classes are out of order
    // (derived first, max depth: promoted custom plugin = 2)
    for (int iteration = 0;  iteration < 2;  iteration++) {
        addCustomWidgetsToWidgetDatabase(custom_widget_list);
        if (custom_widget_list.empty())
            return;
    }
    // Oops, there are classes left whose base class could not be found.
    // Default them to QWidget with warnings.
    const QString fallBackBaseClass = QLatin1String("QWidget");
    for (int i=0; i < custom_widget_list.size(); i++ ) {
        DomCustomWidget *custom_widget = custom_widget_list[i];
        const QString customClassName = custom_widget->elementClass();
        const QString base_class = custom_widget->elementExtends();
        qDebug() << "** WARNING The base class " << base_class << " of the custom widget class " << customClassName
            << " could not be found. Defaulting to " << fallBackBaseClass << '.';
        custom_widget->setElementExtends(fallBackBaseClass);
    }
    // One more pass.
    addCustomWidgetsToWidgetDatabase(custom_widget_list);
    Q_ASSERT(custom_widget_list.empty());
}

DomTabStops *QDesignerResource::saveTabStops()
{
    QDesignerMetaDataBaseItemInterface *item = core()->metaDataBase()->item(m_formWindow);
    Q_ASSERT(item);

    QStringList tabStops;
    foreach (QWidget *widget, item->tabOrder()) {
        if (m_formWindow->mainContainer()->isAncestorOf(widget))
            tabStops.append(widget->objectName());
    }

    if (tabStops.count()) {
        DomTabStops *dom = new DomTabStops;
        dom->setElementTabStop(tabStops);
        return dom;
    }

    return 0;
}

void QDesignerResource::applyTabStops(QWidget *widget, DomTabStops *tabStops)
{
    if (!tabStops)
        return;

    QList<QWidget*> tabOrder;
    foreach (QString widgetName, tabStops->elementTabStop()) {
        if (QWidget *w = qFindChild<QWidget*>(widget, widgetName)) {
            tabOrder.append(w);
        }
    }

    QDesignerMetaDataBaseItemInterface *item = core()->metaDataBase()->item(m_formWindow);
    Q_ASSERT(item);
    item->setTabOrder(tabOrder);
}

/* Unmanaged container pages occur when someone adds a page in a custom widget
 * constructor. They don't have a meta DB entry which causes createDom
 * to return 0. */
inline QString msgUnmanagedPage(QDesignerFormEditorInterface *core,
                                QWidget *container, int index, QWidget *page)
{
    return QCoreApplication::translate("QDesignerResource",
"The container extension of the widget '%1' (%2) returned a widget not managed by Designer '%3' (%4) when queried for page #%5.\n"
"Container pages should only be added by specifying them in XML returned by the domXml() method of the custom widget.").
           arg(container->objectName(), WidgetFactory::classNameOf(core, container),
               page->objectName(), WidgetFactory::classNameOf(core, page)).
           arg(index);
}

DomWidget *QDesignerResource::saveWidget(QWidget *widget, QDesignerContainerExtension *container, DomWidget *ui_parentWidget)
{
    DomWidget *ui_widget = QAbstractFormBuilder::createDom(widget, ui_parentWidget, false);
    QList<DomWidget*> ui_widget_list;

    for (int i=0; i<container->count(); ++i) {
        QWidget *page = container->widget(i);
        Q_ASSERT(page);

        if (DomWidget *ui_page = createDom(page, ui_widget)) {
            ui_widget_list.append(ui_page);
        } else {
            if (QSimpleResource::warningsEnabled())
                designerWarning(msgUnmanagedPage(core(), widget, i, page));
        }
    }

    ui_widget->setElementWidget(ui_widget_list);

    return ui_widget;
}

DomWidget *QDesignerResource::saveWidget(QStackedWidget *widget, DomWidget *ui_parentWidget)
{
    DomWidget *ui_widget = QAbstractFormBuilder::createDom(widget, ui_parentWidget, false);
    QList<DomWidget*> ui_widget_list;
    if (QDesignerContainerExtension *container = qt_extension<QDesignerContainerExtension*>(core()->extensionManager(), widget)) {
        for (int i=0; i<container->count(); ++i) {
            QWidget *page = container->widget(i);
            Q_ASSERT(page);
            if (DomWidget *ui_page = createDom(page, ui_widget)) {
                ui_widget_list.append(ui_page);
            } else {
                if (QSimpleResource::warningsEnabled())
                    designerWarning(msgUnmanagedPage(core(), widget, i, page));
            }
        }
    }

    ui_widget->setElementWidget(ui_widget_list);

    return ui_widget;
}

DomWidget *QDesignerResource::saveWidget(QToolBar *toolBar, DomWidget *ui_parentWidget)
{
    DomWidget *ui_widget = QAbstractFormBuilder::createDom(toolBar, ui_parentWidget, false);
    if (const QMainWindow *mainWindow = qobject_cast<QMainWindow*>(toolBar->parentWidget())) {
        const bool toolBarBreak = mainWindow->toolBarBreak(toolBar);
        const Qt::ToolBarArea area = mainWindow->toolBarArea(toolBar);

        QList<DomProperty*> attributes = ui_widget->elementAttribute();

        DomProperty *attr = new DomProperty();
        attr->setAttributeName(QLatin1String("toolBarArea"));
        attr->setElementEnum(QLatin1String(toolBarAreaMetaEnum().valueToKey(area)));
        attributes  << attr;

        attr = new DomProperty();
        attr->setAttributeName(QLatin1String("toolBarBreak"));
        attr->setElementBool(toolBarBreak ? QLatin1String("true") : QLatin1String("false"));
        attributes  << attr;
        ui_widget->setElementAttribute(attributes);
    }

    return ui_widget;
}

DomWidget *QDesignerResource::saveWidget(QDesignerDockWidget *dockWidget, DomWidget *ui_parentWidget)
{
    DomWidget *ui_widget = QAbstractFormBuilder::createDom(dockWidget, ui_parentWidget, true);
    if (QMainWindow *mainWindow = qobject_cast<QMainWindow*>(dockWidget->parentWidget())) {
        const Qt::DockWidgetArea area = mainWindow->dockWidgetArea(dockWidget);
        DomProperty *attr = new DomProperty();
        attr->setAttributeName(QLatin1String("dockWidgetArea"));
        attr->setElementNumber(int(area));
        ui_widget->setElementAttribute(ui_widget->elementAttribute() << attr);
    }

    return ui_widget;
}

DomWidget *QDesignerResource::saveWidget(QTreeView *treeView, DomWidget *ui_parentWidget)
{
    DomWidget *ui_widget = QAbstractFormBuilder::createDom(treeView, ui_parentWidget, true);

    QDesignerPropertySheetExtension *sheet
            = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), treeView);
    ItemViewPropertySheet *itemViewSheet = static_cast<ItemViewPropertySheet*>(sheet);

    if (itemViewSheet) {
        QHash<QString,QString> nameMap = itemViewSheet->propertyNameMap();
        foreach (const QString &fakeName, nameMap.keys()) {
            int index = itemViewSheet->indexOf(fakeName);
            if (sheet->isChanged(index)) {
                DomProperty *domAttr = createProperty(treeView->header(), nameMap.value(fakeName),
                                                      itemViewSheet->property(index));
                domAttr->setAttributeName(fakeName);
                ui_widget->setElementAttribute(ui_widget->elementAttribute() << domAttr);
            }
        }
    }

    return ui_widget;
}

DomWidget *QDesignerResource::saveWidget(QTableView *tableView, DomWidget *ui_parentWidget)
{
    DomWidget *ui_widget = QAbstractFormBuilder::createDom(tableView, ui_parentWidget, true);

    QDesignerPropertySheetExtension *sheet
            = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), tableView);
    ItemViewPropertySheet *itemViewSheet = static_cast<ItemViewPropertySheet*>(sheet);

    if (itemViewSheet) {
        QHash<QString,QString> nameMap = itemViewSheet->propertyNameMap();
        foreach (const QString &fakeName, nameMap.keys()) {
            int index = itemViewSheet->indexOf(fakeName);
            if (sheet->isChanged(index)) {
                DomProperty *domAttr;
                if (fakeName.startsWith(QLatin1String("horizontal"))) {
                    domAttr = createProperty(tableView->horizontalHeader(), nameMap.value(fakeName),
                                                      itemViewSheet->property(index));
                } else {
                    domAttr = createProperty(tableView->verticalHeader(), nameMap.value(fakeName),
                                                      itemViewSheet->property(index));
                }
                domAttr->setAttributeName(fakeName);
                ui_widget->setElementAttribute(ui_widget->elementAttribute() << domAttr);
            }
        }
    }

    return ui_widget;
}

static void saveStringProperty(DomProperty *property, const PropertySheetStringValue &value)
{
    DomString *str = new DomString();
    str->setText(value.value());

    const QString property_comment = value.disambiguation();
    if (!property_comment.isEmpty())
        str->setAttributeComment(property_comment);
    const QString property_extraComment = value.comment();
    if (!property_extraComment.isEmpty())
        str->setAttributeExtraComment(property_extraComment);
    const bool property_translatable = value.translatable();
    if (!property_translatable)
        str->setAttributeNotr(QLatin1String("true"));

    property->setElementString(str);
}

static void saveKeySequenceProperty(DomProperty *property, const PropertySheetKeySequenceValue &value)
{
    DomString *str = new DomString();
    str->setText(value.value().toString());

    const QString property_comment = value.disambiguation();
    if (!property_comment.isEmpty())
        str->setAttributeComment(property_comment);
    const QString property_extraComment = value.comment();
    if (!property_extraComment.isEmpty())
        str->setAttributeExtraComment(property_extraComment);
    const bool property_translatable = value.translatable();
    if (!property_translatable)
        str->setAttributeNotr(QLatin1String("true"));

    property->setElementString(str);
}

DomWidget *QDesignerResource::saveWidget(QTabWidget *widget, DomWidget *ui_parentWidget)
{
    DomWidget *ui_widget = QAbstractFormBuilder::createDom(widget, ui_parentWidget, false);
    QList<DomWidget*> ui_widget_list;

    if (QDesignerContainerExtension *container = qt_extension<QDesignerContainerExtension*>(core()->extensionManager(), widget)) {
        const int current = widget->currentIndex();
        for (int i=0; i<container->count(); ++i) {
            QWidget *page = container->widget(i);
            Q_ASSERT(page);

            DomWidget *ui_page = createDom(page, ui_widget);
            if (!ui_page) {
                if (QSimpleResource::warningsEnabled())
                    designerWarning(msgUnmanagedPage(core(), widget, i, page));
                continue;
            }
            QList<DomProperty*> ui_attribute_list;

            const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
            // attribute `icon'
            widget->setCurrentIndex(i);
            QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), widget);
            PropertySheetIconValue icon = qVariantValue<PropertySheetIconValue>(sheet->property(sheet->indexOf(QLatin1String("currentTabIcon"))));
            DomProperty *p = resourceBuilder()->saveResource(workingDirectory(), qVariantFromValue(icon));
            if (p) {
                p->setAttributeName(strings.iconAttribute);
                ui_attribute_list.append(p);
            }
            // attribute `title'
            p = textBuilder()->saveText(sheet->property(sheet->indexOf(QLatin1String("currentTabText"))));
            if (p) {
                p->setAttributeName(strings.titleAttribute);
                ui_attribute_list.append(p);
            }

            // attribute `toolTip'
            QVariant v = sheet->property(sheet->indexOf(QLatin1String("currentTabToolTip")));
            if (!qVariantValue<PropertySheetStringValue>(v).value().isEmpty()) {
                p = textBuilder()->saveText(v);
                if (p) {
                    p->setAttributeName(strings.toolTipAttribute);
                ui_attribute_list.append(p);
                }
            }

            // attribute `whatsThis'
            v = sheet->property(sheet->indexOf(QLatin1String("currentTabWhatsThis")));
            if (!qVariantValue<PropertySheetStringValue>(v).value().isEmpty()) {
                p = textBuilder()->saveText(v);
                if (p) {
                    p->setAttributeName(strings.whatsThisAttribute);
                ui_attribute_list.append(p);
                }
            }

            ui_page->setElementAttribute(ui_attribute_list);

            ui_widget_list.append(ui_page);
        }
        widget->setCurrentIndex(current);
    }

    ui_widget->setElementWidget(ui_widget_list);

    return ui_widget;
}

DomWidget *QDesignerResource::saveWidget(QToolBox *widget, DomWidget *ui_parentWidget)
{
    DomWidget *ui_widget = QAbstractFormBuilder::createDom(widget, ui_parentWidget, false);
    QList<DomWidget*> ui_widget_list;

    if (QDesignerContainerExtension *container = qt_extension<QDesignerContainerExtension*>(core()->extensionManager(), widget)) {
        const int current = widget->currentIndex();
        for (int i=0; i<container->count(); ++i) {
            QWidget *page = container->widget(i);
            Q_ASSERT(page);

            DomWidget *ui_page = createDom(page, ui_widget);
            if (!ui_page) {
                if (QSimpleResource::warningsEnabled())
                    designerWarning(msgUnmanagedPage(core(), widget, i, page));
                continue;
            }

            // attribute `label'
            QList<DomProperty*> ui_attribute_list;

            const QFormBuilderStrings &strings = QFormBuilderStrings::instance();

            // attribute `icon'
            widget->setCurrentIndex(i);
            QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), widget);
            PropertySheetIconValue icon = qVariantValue<PropertySheetIconValue>(sheet->property(sheet->indexOf(QLatin1String("currentItemIcon"))));
            DomProperty *p = resourceBuilder()->saveResource(workingDirectory(), qVariantFromValue(icon));
            if (p) {
                p->setAttributeName(strings.iconAttribute);
                ui_attribute_list.append(p);
            }
            p = textBuilder()->saveText(sheet->property(sheet->indexOf(QLatin1String("currentItemText"))));
            if (p) {
                p->setAttributeName(strings.labelAttribute);
                ui_attribute_list.append(p);
            }

            // attribute `toolTip'
            QVariant v = sheet->property(sheet->indexOf(QLatin1String("currentItemToolTip")));
            if (!qVariantValue<PropertySheetStringValue>(v).value().isEmpty()) {
                p = textBuilder()->saveText(v);
                if (p) {
                    p->setAttributeName(strings.toolTipAttribute);
                    ui_attribute_list.append(p);
                }
            }

            ui_page->setElementAttribute(ui_attribute_list);

            ui_widget_list.append(ui_page);
        }
        widget->setCurrentIndex(current);
    }

    ui_widget->setElementWidget(ui_widget_list);

    return ui_widget;
}

DomWidget *QDesignerResource::saveWidget(QWizardPage *wizardPage, DomWidget *ui_parentWidget)
{
    DomWidget *ui_widget = QAbstractFormBuilder::createDom(wizardPage, ui_parentWidget, true);
    QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), wizardPage);
    // Save the page id (string) attribute, append to existing attributes
    const QString pageIdPropertyName = QLatin1String(QWizardPagePropertySheet::pageIdProperty);
    const int pageIdIndex = sheet->indexOf(pageIdPropertyName);
    if (pageIdIndex != -1 && sheet->isChanged(pageIdIndex)) {
        DomProperty *property = variantToDomProperty(this, wizardPage->metaObject(), pageIdPropertyName, sheet->property(pageIdIndex));
        Q_ASSERT(property);
        DomPropertyList attributes = ui_widget->elementAttribute();
        attributes.push_back(property);
        ui_widget->setElementAttribute(attributes);
    }
    return ui_widget;
}

// Do not save the 'currentTabName' properties of containers
static inline bool checkContainerProperty(const QWidget *w, const QString &propertyName)
{
    if (qobject_cast<const QToolBox *>(w))
        return QToolBoxWidgetPropertySheet::checkProperty(propertyName);
    if (qobject_cast<const QTabWidget *>(w))
        return QTabWidgetPropertySheet::checkProperty(propertyName);
    if (qobject_cast<const QStackedWidget *>(w))
        return QStackedWidgetPropertySheet::checkProperty(propertyName);
    if (qobject_cast<const QMdiArea *>(w) || qobject_cast<const QWorkspace *>(w))
        return QMdiAreaPropertySheet::checkProperty(propertyName);
    return true;
}

bool QDesignerResource::checkProperty(QObject *obj, const QString &prop) const
{
    const QDesignerMetaObjectInterface *meta = core()->introspection()->metaObject(obj);

    const int pindex = meta->indexOfProperty(prop);
    if (pindex != -1 && !(meta->property(pindex)->attributes(obj) & QDesignerMetaPropertyInterface::StoredAttribute))
        return false;

    if (prop == QLatin1String("objectName") || prop == QLatin1String("spacerName"))  // ### don't store the property objectName
        return false;

    QWidget *check_widget = 0;
    if (obj->isWidgetType())
        check_widget = static_cast<QWidget*>(obj);

    if (check_widget && prop == QLatin1String("geometry")) {
        if (check_widget == m_formWindow->mainContainer())
            return true; // Save although maincontainer is technically laid-out by embedding container
         if (m_selected && m_selected == check_widget)
             return true;

        return !LayoutInfo::isWidgetLaidout(core(), check_widget);
    }

    if (check_widget && !checkContainerProperty(check_widget, prop))
        return false;

    if (QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), obj)) {
        QDesignerDynamicPropertySheetExtension *dynamicSheet = qt_extension<QDesignerDynamicPropertySheetExtension*>(core()->extensionManager(), obj);
        const int pindex = sheet->indexOf(prop);
        if (sheet->isAttribute(pindex))
            return false;

        if (!dynamicSheet || !dynamicSheet->isDynamicProperty(pindex))
            return sheet->isChanged(pindex);
        if (!sheet->isVisible(pindex))
            return false;
        return true;
    }

    return false;
}

bool QDesignerResource::addItem(DomLayoutItem *ui_item, QLayoutItem *item, QLayout *layout)
{
    if (item->widget() == 0) {
        return false;
    }

    QGridLayout *grid = qobject_cast<QGridLayout*>(layout);
    QBoxLayout *box = qobject_cast<QBoxLayout*>(layout);

    if (grid != 0) {
        const int rowSpan = ui_item->hasAttributeRowSpan() ? ui_item->attributeRowSpan() : 1;
        const int colSpan = ui_item->hasAttributeColSpan() ? ui_item->attributeColSpan() : 1;
        grid->addWidget(item->widget(), ui_item->attributeRow(), ui_item->attributeColumn(), rowSpan, colSpan, item->alignment());
        return true;
    } else if (box != 0) {
        box->addItem(item);
        return true;
    }

    return QAbstractFormBuilder::addItem(ui_item, item, layout);
}

bool QDesignerResource::addItem(DomWidget *ui_widget, QWidget *widget, QWidget *parentWidget)
{
    core()->metaDataBase()->add(widget); // ensure the widget is in the meta database

    if (! QAbstractFormBuilder::addItem(ui_widget, widget, parentWidget) || qobject_cast<QMainWindow*> (parentWidget)) {
        if (QDesignerContainerExtension *container = qt_extension<QDesignerContainerExtension*>(core()->extensionManager(), parentWidget))
            container->addWidget(widget);
    }

    if (QTabWidget *tabWidget = qobject_cast<QTabWidget*>(parentWidget)) {
        const int tabIndex = tabWidget->count() - 1;
        const int current = tabWidget->currentIndex();

        tabWidget->setCurrentIndex(tabIndex);

        const QFormBuilderStrings &strings = QFormBuilderStrings::instance();

        const DomPropertyHash attributes = propertyMap(ui_widget->elementAttribute());
        QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), parentWidget);
        if (DomProperty *picon = attributes.value(strings.iconAttribute)) {
            QVariant v = resourceBuilder()->loadResource(workingDirectory(), picon);
            sheet->setProperty(sheet->indexOf(QLatin1String("currentTabIcon")), v);
        }
        if (DomProperty *ptext = attributes.value(strings.titleAttribute)) {
            QVariant v = textBuilder()->loadText(ptext);
            sheet->setProperty(sheet->indexOf(QLatin1String("currentTabText")), v);
        }
        if (DomProperty *ptext = attributes.value(strings.toolTipAttribute)) {
            QVariant v = textBuilder()->loadText(ptext);
            sheet->setProperty(sheet->indexOf(QLatin1String("currentTabToolTip")), v);
        }
        if (DomProperty *ptext = attributes.value(strings.whatsThisAttribute)) {
            QVariant v = textBuilder()->loadText(ptext);
            sheet->setProperty(sheet->indexOf(QLatin1String("currentTabWhatsThis")), v);
        }
        tabWidget->setCurrentIndex(current);
    } else if (QToolBox *toolBox = qobject_cast<QToolBox*>(parentWidget)) {
        const int itemIndex = toolBox->count() - 1;
        const int current = toolBox->currentIndex();

        toolBox->setCurrentIndex(itemIndex);

        const QFormBuilderStrings &strings = QFormBuilderStrings::instance();

        const DomPropertyHash attributes = propertyMap(ui_widget->elementAttribute());
        QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), parentWidget);
        if (DomProperty *picon = attributes.value(strings.iconAttribute)) {
            QVariant v = resourceBuilder()->loadResource(workingDirectory(), picon);
            sheet->setProperty(sheet->indexOf(QLatin1String("currentItemIcon")), v);
        }
        if (DomProperty *ptext = attributes.value(strings.labelAttribute)) {
            QVariant v = textBuilder()->loadText(ptext);
            sheet->setProperty(sheet->indexOf(QLatin1String("currentItemText")), v);
        }
        if (DomProperty *ptext = attributes.value(strings.toolTipAttribute)) {
            QVariant v = textBuilder()->loadText(ptext);
            sheet->setProperty(sheet->indexOf(QLatin1String("currentItemToolTip")), v);
        }
        toolBox->setCurrentIndex(current);
    }

    return true;
}

bool QDesignerResource::copy(QIODevice *dev, const FormBuilderClipboard &selection)
{
    m_copyWidget = true;

    DomUI *ui = copy(selection);

    m_laidout.clear();
    m_copyWidget = false;

    if (!ui)
        return false;

    QXmlStreamWriter writer(dev);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(1);
    writer.writeStartDocument();
    ui->write(writer);
    writer.writeEndDocument();
    delete ui;
    return true;
}

DomUI *QDesignerResource::copy(const FormBuilderClipboard &selection)
{
    if (selection.empty())
        return 0;

    m_copyWidget = true;

    DomWidget *ui_widget = new DomWidget();
    ui_widget->setAttributeName(QLatin1String(clipboardObjectName));
    bool hasItems = false;
    // Widgets
    if (!selection.m_widgets.empty()) {
        QList<DomWidget*> ui_widget_list;
        const int size = selection.m_widgets.size();
        for (int i=0; i< size; ++i) {
            QWidget *w = selection.m_widgets.at(i);
            m_selected = w;
            DomWidget *ui_child = createDom(w, ui_widget);
            m_selected = 0;
            if (ui_child)
                ui_widget_list.append(ui_child);
        }
        if (!ui_widget_list.empty()) {
            ui_widget->setElementWidget(ui_widget_list);
            hasItems = true;
        }
    }
    // actions
    if (!selection.m_actions.empty()) {
        QList<DomAction*> domActions;
        foreach(QAction* action, selection.m_actions)
            if (DomAction *domAction = createDom(action))
                domActions += domAction;
        if (!domActions.empty()) {
            ui_widget-> setElementAction(domActions);
            hasItems = true;
        }
    }

    m_laidout.clear();
    m_copyWidget = false;

    if (!hasItems) {
        delete ui_widget;
        return 0;
    }
    // UI
    DomUI *ui = new DomUI();
    ui->setAttributeVersion(QLatin1String(currentUiVersion));
    ui->setElementWidget(ui_widget);
    ui->setElementResources(saveResources(m_resourceBuilder->usedQrcFiles()));
    if (DomCustomWidgets *cws = saveCustomWidgets())
        ui->setElementCustomWidgets(cws);
    return ui;
}

FormBuilderClipboard QDesignerResource::paste(DomUI *ui, QWidget *widgetParent, QObject *actionParent)
{
    QDesignerWidgetItemInstaller wii; // Make sure we use QDesignerWidgetItem.
    const int saved = m_isMainWidget;
    m_isMainWidget = false;

    FormBuilderClipboard rc;

    // Widgets
    const DomWidget *topLevel = ui->elementWidget();
    initialize(ui);
    const QList<DomWidget*> domWidgets = topLevel->elementWidget();
    if (!domWidgets.empty()) {
        const QPoint offset = m_formWindow->grid();
        foreach (DomWidget* domWidget, domWidgets) {
            if (QWidget *w = create(domWidget, widgetParent)) {
                w->move(w->pos() + offset);
                // ### change the init properties of w
                rc.m_widgets.append(w);
            }
        }
    }
    const QList<DomAction*> domActions = topLevel->elementAction();
    if (!domActions.empty())
        foreach (DomAction *domAction, domActions)
            if (QAction *a = create(domAction, actionParent))
                rc.m_actions .append(a);

    m_isMainWidget = saved;

    if (QDesignerExtraInfoExtension *extra = qt_extension<QDesignerExtraInfoExtension*>(core()->extensionManager(), core()))
        extra->loadUiExtraInfo(ui);

    createResources(ui->elementResources());

    return rc;
}

FormBuilderClipboard QDesignerResource::paste(QIODevice *dev, QWidget *widgetParent, QObject *actionParent)
{
    DomUI ui;
    QXmlStreamReader reader(dev);
    bool uiInitialized = false;

    const QString uiElement = QLatin1String("ui");
    while (!reader.atEnd()) {
        if (reader.readNext() == QXmlStreamReader::StartElement) {
            if (reader.name().compare(uiElement, Qt::CaseInsensitive)) {
                ui.read(reader);
                uiInitialized = true;
            } else {
                //: Parsing clipboard contents
                reader.raiseError(QCoreApplication::translate("QDesignerResource", "Unexpected element <%1>").arg(reader.name().toString()));
            }
        }
    }
    if (reader.hasError()) {
        //: Parsing clipboard contents
        designerWarning(QCoreApplication::translate("QDesignerResource", "Error while pasting clipboard contents at line %1, column %2: %3")
                                    .arg(reader.lineNumber()).arg(reader.columnNumber())
                                    .arg(reader.errorString()));
        uiInitialized = false;
    } else if (uiInitialized == false) {
        //: Parsing clipboard contents
        designerWarning(QCoreApplication::translate("QDesignerResource", "Error while pasting clipboard contents: The root element <ui> is missing."));
    }

    if (!uiInitialized)
        return FormBuilderClipboard();

    FormBuilderClipboard clipBoard = paste(&ui, widgetParent, actionParent);

    return clipBoard;
}

void QDesignerResource::layoutInfo(DomLayout *layout, QObject *parent, int *margin, int *spacing)
{
    QAbstractFormBuilder::layoutInfo(layout, parent, margin, spacing);
}

DomCustomWidgets *QDesignerResource::saveCustomWidgets()
{
    if (m_usedCustomWidgets.isEmpty())
        return 0;

    // We would like the list to be in order of the widget database indexes
    // to ensure that base classes come first (nice optics)
    QDesignerFormEditorInterface *core = m_formWindow->core();
    QDesignerWidgetDataBaseInterface *db = core->widgetDataBase();
    const bool isInternalWidgetDataBase = qobject_cast<const WidgetDataBase *>(db);
    typedef QMap<int,DomCustomWidget*>  OrderedDBIndexDomCustomWidgetMap;
    OrderedDBIndexDomCustomWidgetMap orderedMap;

    const QString global = QLatin1String("global");
    foreach (QDesignerWidgetDataBaseItemInterface *item, m_usedCustomWidgets.keys()) {
        const QString name = item->name();
        DomCustomWidget *custom_widget = new DomCustomWidget;

        custom_widget->setElementClass(name);
        if (item->isContainer())
            custom_widget->setElementContainer(item->isContainer());

        if (!item->includeFile().isEmpty()) {
            DomHeader *header = new DomHeader;
            const  IncludeSpecification spec = includeSpecification(item->includeFile());
            header->setText(spec.first);
            if (spec.second == IncludeGlobal) {
                header->setAttributeLocation(global);
            }
            custom_widget->setElementHeader(header);
            custom_widget->setElementExtends(item->extends());
        }

        if (isInternalWidgetDataBase) {
            WidgetDataBaseItem *internalItem = static_cast<WidgetDataBaseItem *>(item);
            const QStringList fakeSlots = internalItem->fakeSlots();
            const QStringList fakeSignals = internalItem->fakeSignals();
            if (!fakeSlots.empty() || !fakeSignals.empty()) {
                DomSlots *domSlots = new DomSlots();
                domSlots->setElementSlot(fakeSlots);
                domSlots->setElementSignal(fakeSignals);
                custom_widget->setElementSlots(domSlots);
            }
            const QString addPageMethod = internalItem->addPageMethod();
            if (!addPageMethod.isEmpty())
                custom_widget->setElementAddPageMethod(addPageMethod);
        }

        // Look up static per-class scripts of designer
        if (DomScript *domScript = createScript(customWidgetScript(core, name), ScriptCustomWidgetPlugin))
            custom_widget->setElementScript(domScript);

        orderedMap.insert(db->indexOfClassName(name), custom_widget);
    }

    DomCustomWidgets *customWidgets = new DomCustomWidgets;
    customWidgets->setElementCustomWidget(orderedMap.values());
    return customWidgets;
}

bool QDesignerResource::canCompressMargins(QObject *object) const
{
    if (QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), object)) {
        if (qobject_cast<QLayout *>(object)) {
            const int l = sheet->property(sheet->indexOf(QLatin1String("leftMargin"))).toInt();
            const int t = sheet->property(sheet->indexOf(QLatin1String("topMargin"))).toInt();
            const int r = sheet->property(sheet->indexOf(QLatin1String("rightMargin"))).toInt();
            const int b = sheet->property(sheet->indexOf(QLatin1String("bottomMargin"))).toInt();
            if (l == t && l == r && l == b)
                return true;
        }
    }
    return false;
}

bool QDesignerResource::canCompressSpacings(QObject *object) const
{
    if (QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), object)) {
        if (qobject_cast<QGridLayout *>(object)) {
            const int h = sheet->property(sheet->indexOf(QLatin1String("horizontalSpacing"))).toInt();
            const int v = sheet->property(sheet->indexOf(QLatin1String("verticalSpacing"))).toInt();
            if (h == v)
                return true;
        }
    }
    return false;
}

QList<DomProperty*> QDesignerResource::computeProperties(QObject *object)
{
    QList<DomProperty*> properties;
    if (QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), object)) {
        QDesignerDynamicPropertySheetExtension *dynamicSheet = qt_extension<QDesignerDynamicPropertySheetExtension*>(core()->extensionManager(), object);
        const int count = sheet->count();
        QList<DomProperty *> marginProperties;
        QList<DomProperty *> spacingProperties;
        const bool compressMargins = canCompressMargins(object);
        const bool compressSpacings = canCompressSpacings(object);
        for (int index = 0; index < count; ++index) {
            if (!sheet->isChanged(index) && (!dynamicSheet || !dynamicSheet->isDynamicProperty(index)))
                continue;

            const QString propertyName = sheet->propertyName(index);
            // Suppress windowModality in legacy forms that have it set on child widgets
            if (propertyName == QLatin1String("windowModality") && !sheet->isVisible(index))
                continue;

            const QVariant value = sheet->property(index);
            if (DomProperty *p = createProperty(object, propertyName, value)) {
                if (compressMargins && (propertyName == QLatin1String("leftMargin")
                        || propertyName == QLatin1String("rightMargin")
                        || propertyName == QLatin1String("topMargin")
                        || propertyName == QLatin1String("bottomMargin"))) {
                    marginProperties.append(p);
                } else if (compressSpacings && (propertyName == QLatin1String("horizontalSpacing")
                        || propertyName == QLatin1String("verticalSpacing"))) {
                    spacingProperties.append(p);
                } else {
                    properties.append(p);
                }
            }
        }
        if (compressMargins) {
            if (marginProperties.count() == 4) { // if we have 3 it means one is reset so we can't compress
                DomProperty *marginProperty = marginProperties.at(0);
                marginProperty->setAttributeName(QLatin1String("margin"));
                properties.append(marginProperty);
                delete marginProperties.at(1);
                delete marginProperties.at(2);
                delete marginProperties.at(3);
            } else {
                properties += marginProperties;
            }
        }
        if (compressSpacings) {
            if (spacingProperties.count() == 2) {
                DomProperty *spacingProperty = spacingProperties.at(0);
                spacingProperty->setAttributeName(QLatin1String("spacing"));
                properties.append(spacingProperty);
                delete spacingProperties.at(1);
            } else {
                properties += spacingProperties;
            }
        }
    }
    return properties;
}

DomProperty *QDesignerResource::applyProperStdSetAttribute(QObject *object, const QString &propertyName, DomProperty *property)
{
    if (!property)
        return 0;

    QExtensionManager *mgr = core()->extensionManager();
    if (const QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(mgr, object)) {
        const QDesignerDynamicPropertySheetExtension *dynamicSheet = qt_extension<QDesignerDynamicPropertySheetExtension*>(mgr, object);
        const QDesignerPropertySheet *designerSheet = qobject_cast<QDesignerPropertySheet*>(core()->extensionManager()->extension(object, Q_TYPEID(QDesignerPropertySheetExtension)));
        const int index = sheet->indexOf(propertyName);
        if ((dynamicSheet && dynamicSheet->isDynamicProperty(index)) || (designerSheet && designerSheet->isDefaultDynamicProperty(index)))
            property->setAttributeStdset(0);
    }
    return property;
}

// Optimistic check for a standard setter function
static inline bool hasSetter(QDesignerFormEditorInterface *core, QObject *object, const QString &propertyName)
{
    const QDesignerMetaObjectInterface *meta = core->introspection()->metaObject(object);
    const int pindex = meta->indexOfProperty(propertyName);
    if (pindex == -1)
        return true;
    return  meta->property(pindex)->hasSetter();
}

DomProperty *QDesignerResource::createProperty(QObject *object, const QString &propertyName, const QVariant &value)
{
    if (!checkProperty(object, propertyName)) {
        return 0;
    }

    if (qVariantCanConvert<PropertySheetFlagValue>(value)) {
        const PropertySheetFlagValue f = qVariantValue<PropertySheetFlagValue>(value);
        const QString flagString = f.metaFlags.toString(f.value, DesignerMetaFlags::FullyQualified);
        if (flagString.isEmpty())
            return 0;

        DomProperty *p = new DomProperty;
        // check if we have a standard cpp set function
        if (!hasSetter(core(), object, propertyName))
            p->setAttributeStdset(0);
        p->setAttributeName(propertyName);
        p->setElementSet(flagString);
        return applyProperStdSetAttribute(object, propertyName, p);
    } else if (qVariantCanConvert<PropertySheetEnumValue>(value)) {
        const PropertySheetEnumValue e = qVariantValue<PropertySheetEnumValue>(value);
        bool ok;
        const QString id = e.metaEnum.toString(e.value, DesignerMetaEnum::FullyQualified, &ok);
        if (!ok)
            designerWarning(e.metaEnum.messageToStringFailed(e.value));
        if (id.isEmpty())
            return 0;

        DomProperty *p = new DomProperty;
        // check if we have a standard cpp set function
        if (!hasSetter(core(), object, propertyName))
            p->setAttributeStdset(0);
        p->setAttributeName(propertyName);
        p->setElementEnum(id);
        return applyProperStdSetAttribute(object, propertyName, p);
    } else if (qVariantCanConvert<PropertySheetStringValue>(value)) {
        const PropertySheetStringValue strVal = qVariantValue<PropertySheetStringValue>(value);
        DomProperty *p = new DomProperty;
        if (!hasSetter(core(), object, propertyName))
            p->setAttributeStdset(0);

        p->setAttributeName(propertyName);

        saveStringProperty(p, strVal);

        return applyProperStdSetAttribute(object, propertyName, p);
    } else if (qVariantCanConvert<PropertySheetKeySequenceValue>(value)) {
        const PropertySheetKeySequenceValue keyVal = qVariantValue<PropertySheetKeySequenceValue>(value);
        DomProperty *p = new DomProperty;
        if (!hasSetter(core(), object, propertyName))
            p->setAttributeStdset(0);

        p->setAttributeName(propertyName);

        saveKeySequenceProperty(p, keyVal);

        return applyProperStdSetAttribute(object, propertyName, p);
    }

    return applyProperStdSetAttribute(object, propertyName, QAbstractFormBuilder::createProperty(object, propertyName, value));
}

QStringList QDesignerResource::mergeWithLoadedPaths(const QStringList &paths) const
{
    QStringList newPaths = paths;
#ifdef OLD_RESOURCE_FORMAT
    QStringList loadedPaths = m_resourceBuilder->loadedQrcFiles();
    QStringListIterator it(loadedPaths);
    while (it.hasNext()) {
        const QString path = it.next();
        if (!newPaths.contains(path))
            newPaths << path;
    }
#endif
    return newPaths;
}


void QDesignerResource::createResources(DomResources *resources)
{
    QStringList paths;
    if (resources != 0) {
        const QList<DomResource*> dom_include = resources->elementInclude();
        foreach (DomResource *res, dom_include) {
            QString path = QDir::cleanPath(m_formWindow->absoluteDir().absoluteFilePath(res->attributeLocation()));
            while (!QFile::exists(path)) {
                QWidget *dialogParent = m_formWindow->core()->topLevel();
                const QString promptTitle = QApplication::translate("qdesigner_internal::QDesignerResource", "Loading qrc file", 0, QApplication::UnicodeUTF8);
                const QString prompt = QApplication::translate("qdesigner_internal::QDesignerResource", "The specified qrc file <p><b>%1</b></p><p>could not be found. Do you want to update the file location?</p>", 0, QApplication::UnicodeUTF8).arg(path);

                const QMessageBox::StandardButton answer = core()->dialogGui()->message(dialogParent,  QDesignerDialogGuiInterface::ResourceLoadFailureMessage,
                        QMessageBox::Warning, promptTitle,  prompt, QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
                if (answer == QMessageBox::Yes) {
                    const QFileInfo fi(path);
                    const QString fileDialogTitle = QApplication::translate("qdesigner_internal::QDesignerResource", "New location for %1", 0, QApplication::UnicodeUTF8).arg(fi.fileName());
                    const QString fileDialogPattern = QApplication::translate("qdesigner_internal::QDesignerResource", "Resource files (*.qrc)", 0, QApplication::UnicodeUTF8);
                    path = core()->dialogGui()->getOpenFileName(dialogParent, fileDialogTitle, fi.absolutePath(), fileDialogPattern);
                    if (path.isEmpty())
                        break;
                } else {
                    break;
                }
            }
            if (!path.isEmpty()) {
                paths << path;
                m_formWindow->addResourceFile(path);
            }
        }
    }

#ifdef OLD_RESOURCE_FORMAT
    paths = mergeWithLoadedPaths(paths);
#endif

    QtResourceSet *resourceSet = m_formWindow->resourceSet();
    if (resourceSet) {
        QStringList oldPaths = resourceSet->activeQrcPaths();
        QStringList newPaths = oldPaths;
        QStringListIterator it(paths);
        while (it.hasNext()) {
            const QString path = it.next();
            if (!newPaths.contains(path))
                newPaths << path;
        }
        resourceSet->activateQrcPaths(newPaths);
    } else {
        resourceSet = m_formWindow->core()->resourceModel()->addResourceSet(paths);
        m_formWindow->setResourceSet(resourceSet);
        QObject::connect(m_formWindow->core()->resourceModel(), SIGNAL(resourceSetActivated(QtResourceSet *, bool)),
                m_formWindow, SLOT(resourceSetActivated(QtResourceSet *, bool)));
    }
}

DomResources *QDesignerResource::saveResources()
{
    QStringList paths;
    if (m_formWindow->saveResourcesBehaviour() == FormWindowBase::SaveAll) {
        QtResourceSet *resourceSet = m_formWindow->resourceSet();
        QList<DomResource*> dom_include;
        if (resourceSet)
            paths = resourceSet->activeQrcPaths();
    } else if (m_formWindow->saveResourcesBehaviour() == FormWindowBase::SaveOnlyUsedQrcFiles) {
        paths = m_resourceBuilder->usedQrcFiles();
    }

    return saveResources(paths);
}

DomResources *QDesignerResource::saveResources(const QStringList &qrcPaths)
{
    QtResourceSet *resourceSet = m_formWindow->resourceSet();
    QList<DomResource*> dom_include;
    if (resourceSet) {
        const QStringList activePaths = resourceSet->activeQrcPaths();
        foreach (QString path, activePaths) {
            if (qrcPaths.contains(path)) {
                DomResource *dom_res = new DomResource;
                QString conv_path = path;
                if (m_resourceBuilder->isSaveRelative())
                    conv_path = m_formWindow->absoluteDir().relativeFilePath(path);
                dom_res->setAttributeLocation(conv_path.replace(QDir::separator(), QLatin1Char('/')));
                dom_include.append(dom_res);
            }
        }
    }

    DomResources *dom_resources = new DomResources;
    dom_resources->setElementInclude(dom_include);

    return dom_resources;
}

DomAction *QDesignerResource::createDom(QAction *action)
{
    if (!core()->metaDataBase()->item(action) || action->menu())
        return 0;

    return QAbstractFormBuilder::createDom(action);
}

DomActionGroup *QDesignerResource::createDom(QActionGroup *actionGroup)
{
    if (core()->metaDataBase()->item(actionGroup) != 0) {
        return QAbstractFormBuilder::createDom(actionGroup);
    }

    return 0;
}

QAction *QDesignerResource::create(DomAction *ui_action, QObject *parent)
{
    if (QAction *action = QAbstractFormBuilder::create(ui_action, parent)) {
        core()->metaDataBase()->add(action);
        return action;
    }

    return 0;
}

QActionGroup *QDesignerResource::create(DomActionGroup *ui_action_group, QObject *parent)
{
    if (QActionGroup *actionGroup = QAbstractFormBuilder::create(ui_action_group, parent)) {
        core()->metaDataBase()->add(actionGroup);
        return actionGroup;
    }

    return 0;
}

DomActionRef *QDesignerResource::createActionRefDom(QAction *action)
{
    if (!core()->metaDataBase()->item(action)
            || (!action->isSeparator() && !action->menu() && action->objectName().isEmpty()))
        return 0;

    return QAbstractFormBuilder::createActionRefDom(action);
}

void QDesignerResource::addMenuAction(QAction *action)
{
    core()->metaDataBase()->add(action);
}

QAction *QDesignerResource::createAction(QObject *parent, const QString &name)
{
    if (QAction *action = QAbstractFormBuilder::createAction(parent, name)) {
        core()->metaDataBase()->add(action);
        return action;
    }

    return 0;
}

QActionGroup *QDesignerResource::createActionGroup(QObject *parent, const QString &name)
{
    if (QActionGroup *actionGroup = QAbstractFormBuilder::createActionGroup(parent, name)) {
        core()->metaDataBase()->add(actionGroup);
        return actionGroup;
    }

    return 0;
}

/* Apply the attributes to a widget via property sheet where appropriate,
 * that is, the sheet handles attributive fake properties */
void QDesignerResource::applyAttributesToPropertySheet(const DomWidget *ui_widget, QWidget *widget)
{
    const DomPropertyList attributes = ui_widget->elementAttribute();
    if (attributes.empty())
        return;
    QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(m_formWindow->core()->extensionManager(), widget);
    const DomPropertyList::const_iterator acend = attributes.constEnd();
    for (DomPropertyList::const_iterator it = attributes.constBegin(); it != acend; ++it) {
        const QString name = (*it)->attributeName();
        const int index = sheet->indexOf(name);
        if (index == -1) {
            const QString msg = QString::fromUtf8("Unable to apply attributive property '%1' to '%2'. It does not exist.").arg(name, widget->objectName());
            designerWarning(msg);
        } else {
            sheet->setProperty(index, domPropertyToVariant(this, widget->metaObject(), *it));
            sheet->setChanged(index, true);
        }
    }
}

void QDesignerResource::loadExtraInfo(DomWidget *ui_widget, QWidget *widget, QWidget *parentWidget)
{
    QAbstractFormBuilder::loadExtraInfo(ui_widget, widget, parentWidget);
    // Apply the page id attribute of a QWizardPage (which is an  attributive fake property)
    if (qobject_cast<const QWizardPage*>(widget))
        applyAttributesToPropertySheet(ui_widget, widget);
}

// Add user defined scripts (dialog box) belonging to QWidget to DomWidget.
void QDesignerResource::addUserDefinedScripts(QWidget *w, DomWidget *ui_widget)
{
    QDesignerFormEditorInterface *core = m_formWindow->core();
    DomScripts domScripts = ui_widget->elementScript();
    // Look up user-defined scripts of designer
    if (const qdesigner_internal::MetaDataBase *metaDataBase = qobject_cast<const qdesigner_internal::MetaDataBase *>(core->metaDataBase())) {
        if (const qdesigner_internal::MetaDataBaseItem *metaItem = metaDataBase->metaDataBaseItem(w)) {
            addScript(metaItem->script(), ScriptDesigner, domScripts);
        }
    }
    if (!domScripts.empty())
        ui_widget->setElementScript(domScripts);
}
}

QT_END_NAMESPACE
