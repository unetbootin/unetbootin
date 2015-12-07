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

#include "newform.h"
#include "qdesigner_workbench.h"
#include "qdesigner_actions.h"
#include "qdesigner_formwindow.h"
#include "qdesigner_settings.h"

#include <qdesigner_formbuilder_p.h>
#include <sheet_delegate_p.h>
#include <widgetdatabase_p.h>

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerLanguageExtension>
#include <QtDesigner/QDesignerWidgetDataBaseInterface>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>
#include <QtCore/QByteArray>
#include <QtCore/QBuffer>
#include <QtCore/QDir>
#include <QtCore/QTemporaryFile>

#include <QtGui/QHeaderView>
#include <QtGui/QPainter>
#include <QtGui/QPushButton>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>

QT_BEGIN_NAMESPACE

enum NewForm_CustomRole {
    // File name (templates from resources, paths)
    TemplateNameRole = Qt::UserRole + 100,
    // Class name (widgets from Widget data base)
    ClassNameRole = Qt::UserRole + 101
};

static const char *newFormObjectNameC = "Form";

NewForm::NewForm(QDesignerWorkbench *workbench, QWidget *parentWidget, const QString &fileName)
    : QDialog(parentWidget,
#ifdef Q_WS_MAC
            Qt::Tool |
#endif
            Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
      m_workbench(workbench),
      m_createButton(new QPushButton(QApplication::translate("NewForm", "C&reate", 0, QApplication::UnicodeUTF8))),
      m_recentButton(new QPushButton(QApplication::translate("NewForm", "Recent", 0,  QApplication::UnicodeUTF8))),
      m_fileName(fileName),
      m_currentItem(0),
      m_acceptedItem(0)
{
    QDesignerFormEditorInterface *core = workbench->core();
    ui.setupUi(this);
    ui.treeWidget->setItemDelegate(new qdesigner_internal::SheetDelegate(ui.treeWidget, this));
    ui.treeWidget->header()->hide();
    ui.treeWidget->header()->setStretchLastSection(true);
    ui.lblPreview->setBackgroundRole(QPalette::Base);
    QDesignerSettings settings;
    ui.chkShowOnStartup->setChecked(settings.showNewFormOnStartup());
    ui.buttonBox->clear();
    ui.buttonBox->addButton(QApplication::translate("NewForm", "&Close", 0,
                                        QApplication::UnicodeUTF8), QDialogButtonBox::RejectRole);
    ui.buttonBox->addButton(m_createButton, QDialogButtonBox::AcceptRole);
    m_createButton->setEnabled(false);
    ui.buttonBox->addButton(QApplication::translate("NewForm", "&Open...", 0,
                                    QApplication::UnicodeUTF8), QDialogButtonBox::ActionRole);
    ui.buttonBox->addButton(m_recentButton, QDialogButtonBox::ActionRole);
    QDesignerActions *da = workbench->actionManager();
    QMenu *recentFilesMenu = new QMenu(tr("&Recent Forms"), m_recentButton);
    // Pop the "Recent Files" stuff in here.
    foreach(QAction *recentAction, da->recentFilesActions()->actions()) {
        recentFilesMenu->addAction(recentAction);
        connect(recentAction, SIGNAL(triggered()), this, SLOT(recentFileChosen()));
    }
    m_recentButton->setMenu(recentFilesMenu);


    QString uiExtension = QLatin1String("ui");
    QString templatePath = QLatin1String(":/trolltech/designer/templates/forms");

    QDesignerLanguageExtension *lang = qt_extension<QDesignerLanguageExtension *>(core->extensionManager(), core);
    if (lang) {
        templatePath = QLatin1String(":/templates/forms");
        uiExtension = lang->uiExtension();
    }

    // Resource templates
    const QString formTemplate = settings.formTemplate();
    QTreeWidgetItem *selectedItem = 0;
    loadFrom(templatePath, true, uiExtension, formTemplate, selectedItem);
    // Additional template paths
    const QStringList formTemplatePaths = settings.formTemplatePaths();
    const QStringList::const_iterator ftcend = formTemplatePaths.constEnd();
    for (QStringList::const_iterator it = formTemplatePaths.constBegin(); it != ftcend; ++it)
        loadFrom(*it, false, uiExtension, formTemplate, selectedItem);

    // Widgets/custom widgets
    if (!lang) {
        loadFrom(tr("Widgets"), qdesigner_internal::WidgetDataBase::formWidgetClasses(core), formTemplate, selectedItem);
        loadFrom(tr("Custom Widgets"), qdesigner_internal::WidgetDataBase::customFormWidgetClasses(core), formTemplate, selectedItem);
    }

    // Still no selection - default to first item
    if (selectedItem == 0 && ui.treeWidget->topLevelItemCount() != 0) {
        QTreeWidgetItem *firstTopLevel = ui.treeWidget->topLevelItem(0);
        if (firstTopLevel->childCount() > 0)
            selectedItem = firstTopLevel->child(0);
    }

    // Open parent, select and make visible
    if (selectedItem) {
        ui.treeWidget->setCurrentItem(selectedItem);
        ui.treeWidget->setItemSelected(selectedItem, true);
        ui.treeWidget->scrollToItem(selectedItem->parent());
    }
}

NewForm::~NewForm()
{
    QDesignerSettings settings;
    settings.setShowNewFormOnStartup(ui.chkShowOnStartup->isChecked());
    // Do not change previously stored item if dialog was rejected
    if (m_acceptedItem)
        settings.setFormTemplate(m_acceptedItem->text(0));
}

void NewForm::recentFileChosen()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action)
        return;
    if (action->objectName() == QLatin1String("__qt_action_clear_menu_"))
        return;
    close();
}

void NewForm::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *)
{
    if (!current)
        return;

    if (!current->parent()) { // Top level item: Ensure expanded when browsing down
        return;
    }

    m_currentItem = current;

    const QPixmap pixmap = formPreviewPixmap(m_currentItem);
    if (pixmap.isNull()) {
        m_createButton->setEnabled(false);
        ui.lblPreview->setText(tr("Error loading form"));
    } else {
        m_createButton->setEnabled(true);
        m_createButton->setDefault(true);
        ui.lblPreview->setPixmap(pixmap);
    }
}

void NewForm::on_treeWidget_itemActivated(QTreeWidgetItem *item)
{
    if (item->data(0, TemplateNameRole).isValid() || item->data(0, ClassNameRole).isValid())
        m_createButton->animateClick(0);
}

void NewForm::on_buttonBox_clicked(QAbstractButton *btn)
{
    switch (ui.buttonBox->buttonRole(btn)) {
    case QDialogButtonBox::RejectRole:
        reject();
        break;
    case QDialogButtonBox::ActionRole:
        if (btn != m_recentButton) {
            m_fileName.clear();
            if (m_workbench->actionManager()->openForm(this))
                accept();
        }
        break;
    case QDialogButtonBox::AcceptRole:
        if (m_currentItem) {
            QString errorMessage;
            if (openTemplate(m_currentItem, &errorMessage)) {
                accept();
                m_acceptedItem = m_currentItem;
            }  else {
                QMessageBox::warning(this, tr("Read error"), errorMessage);
            }
        }
        break;
    default:
        break;
    }
}

QDesignerWorkbench *NewForm::workbench() const
{
    return m_workbench;
}

QPixmap  NewForm::formPreviewPixmap(const QTreeWidgetItem *item)
{
    // Cache pixmaps per item
    ItemPixmapMap::iterator it = m_itemPixmapMap.find(item);
    if (it == m_itemPixmapMap.end()) {
        // file or string?
        const QVariant fileName = item->data(0, TemplateNameRole);
        QPixmap rc;
        if (fileName.type() == QVariant::String) {
            rc = formPreviewPixmap(fileName.toString());
        } else {
            const QVariant className = item->data(0, ClassNameRole);
            Q_ASSERT(className.type() == QVariant::String);
            QByteArray data =  qdesigner_internal::WidgetDataBase::formTemplate(m_workbench->core(), className.toString(), QLatin1String(newFormObjectNameC)).toUtf8();
            QBuffer buffer(&data);
            rc = formPreviewPixmap(buffer);
        }
        if (rc.isNull()) // Retry invalid ones
            return rc;
        it = m_itemPixmapMap.insert(item, rc);
    }
    return it.value();
}

QPixmap NewForm::formPreviewPixmap(const QString &fileName) const
{
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QFileInfo fi(fileName);
        const QPixmap rc = formPreviewPixmap(f, fi.absolutePath());
        f.close();
        return rc;
    }
    qWarning() << "The file " << fileName << " could not be opened: " << f.errorString();
    return QPixmap();
}

QPixmap NewForm::formPreviewPixmap(QIODevice &file, const QString &workingDir) const
{
    qdesigner_internal::QDesignerFormBuilder formBuilder(m_workbench->core(), qdesigner_internal::QDesignerFormBuilder::DisableScripts);
    if (!workingDir.isEmpty())
        formBuilder.setWorkingDirectory(workingDir);

    QWidget *widget = formBuilder.load(&file, 0);
    if (!widget)
        return QPixmap();

    const int margin = 7;
    const int shadow = 7;
    const int previewSize = 256;
    const QPixmap pixmap = QPixmap::grabWidget(widget);
    widget->deleteLater();

    const QImage image = pixmap.toImage().scaled(previewSize - margin * 2, previewSize - margin * 2,
                                                                      Qt::KeepAspectRatio,
                                                                      Qt::SmoothTransformation);

    QImage dest(previewSize, previewSize, QImage::Format_ARGB32_Premultiplied);
    dest.fill(0);

    QPainter p(&dest);
    p.drawImage(margin, margin, image);

    p.setPen(QPen(palette().brush(QPalette::WindowText), 0));

    p.drawRect(margin-1, margin-1, image.width() + 1, image.height() + 1);

    const QColor dark(Qt::darkGray);
    const QColor light(Qt::transparent);

    // right shadow
    {
        const QRect rect(margin + image.width() + 1, margin + shadow, shadow, image.height() - shadow + 1);
        QLinearGradient lg(rect.topLeft(), rect.topRight());
        lg.setColorAt(0, dark);
        lg.setColorAt(1, light);
        p.fillRect(rect, lg);
    }

    // bottom shadow
    {
        const QRect rect(margin + shadow, margin + image.height() + 1, image.width() - shadow + 1, shadow);
        QLinearGradient lg(rect.topLeft(), rect.bottomLeft());
        lg.setColorAt(0, dark);
        lg.setColorAt(1, light);
        p.fillRect(rect, lg);
    }

    // bottom/right corner shadow
    {
        const QRect rect(margin + image.width() + 1, margin + image.height() + 1, shadow, shadow);
        QRadialGradient g(rect.topLeft(), shadow);
        g.setColorAt(0, dark);
        g.setColorAt(1, light);
        p.fillRect(rect, g);
    }

    // top/right corner
    {
        const QRect rect(margin + image.width() + 1, margin, shadow, shadow);
        QRadialGradient g(rect.bottomLeft(), shadow);
        g.setColorAt(0, dark);
        g.setColorAt(1, light);
        p.fillRect(rect, g);
    }

    // bottom/left corner
    {
        const QRect rect(margin, margin + image.height() + 1, shadow, shadow);
        QRadialGradient g(rect.topRight(), shadow);
        g.setColorAt(0, dark);
        g.setColorAt(1, light);
        p.fillRect(rect, g);
    }

    p.end();

    return QPixmap::fromImage(dest);
}

void NewForm::loadFrom(const QString &path, bool resourceFile, const QString &uiExtension,
                       const QString &selectedItem, QTreeWidgetItem *&selectedItemFound)
{
    const QDir dir(path);

    if (!dir.exists())
        return;

    // Iterate through the directory and add the templates
    const QFileInfoList list = dir.entryInfoList(QStringList(QLatin1String("*.") + uiExtension),
                                                 QDir::Files);

    if (list.isEmpty())
        return;

    const QChar separator = resourceFile ? QChar(QLatin1Char('/'))
                                         : QDir::separator();
    QTreeWidgetItem *root = new QTreeWidgetItem(ui.treeWidget);
    root->setFlags(root->flags() & ~Qt::ItemIsSelectable);
    // Try to get something that is easy to read.
    QString visiblePath = path;
    int index = visiblePath.lastIndexOf(separator);
    if (index != -1) {
        // try to find a second slash, just to be a bit better.
        const int index2 = visiblePath.lastIndexOf(separator, index - 1);
        if (index2 != -1)
            index = index2;
        visiblePath = visiblePath.mid(index + 1);
        visiblePath = QDir::toNativeSeparators(visiblePath);
    }

    const QChar underscore = QLatin1Char('_');
    const QChar blank = QLatin1Char(' ');
    root->setText(0, visiblePath.replace(underscore, blank));
    root->setToolTip(0, path);

    foreach(QFileInfo fi, list) {
        if (!fi.isFile())
            continue;

        QTreeWidgetItem *item = new QTreeWidgetItem(root);
        const QString text = fi.baseName().replace(underscore, blank);
        if (selectedItemFound == 0 && text == selectedItem)
            selectedItemFound = item;
        item->setText(0, text);
        item->setData(0, TemplateNameRole, fi.absoluteFilePath());
    }
}

void NewForm::loadFrom(const QString &title, const QStringList &nameList,
                       const QString &selectedItem, QTreeWidgetItem *&selectedItemFound)
{
    if (nameList.empty())
        return;
    QTreeWidgetItem *root = new QTreeWidgetItem(ui.treeWidget);
    root->setFlags(root->flags() & ~Qt::ItemIsSelectable);
    root->setText(0, title);
    const QStringList::const_iterator cend = nameList.constEnd();
    for (QStringList::const_iterator it = nameList.constBegin(); it != cend; ++it) {
        const QString text = *it;
        QTreeWidgetItem *item = new QTreeWidgetItem(root);
        item->setText(0, text);
        if (selectedItemFound == 0 && text == selectedItem)
            selectedItemFound = item;
        item->setData(0, ClassNameRole, *it);
    }
}


void NewForm::on_treeWidget_itemPressed(QTreeWidgetItem *item)
{
    if (item && !item->parent())
        ui.treeWidget->setItemExpanded(item, !ui.treeWidget->isItemExpanded(item));
}

bool NewForm::openTemplate(const QTreeWidgetItem *item, QString *errorMessage)
{
    // file or string?
    const QVariant templateFileName = item->data(0, TemplateNameRole);
    if (templateFileName.type() == QVariant::String)
        return workbench()->openTemplate(templateFileName.toString(), m_fileName, errorMessage);
    // Content, Write to temporary file
    const QString className = item->data(0, ClassNameRole).toString();
    const QString contents = qdesigner_internal::WidgetDataBase::formTemplate(m_workbench->core(), className, QLatin1String(newFormObjectNameC));
    QString tempPattern = QDir::tempPath();
    if (!tempPattern.endsWith(QDir::separator())) // platform-dependant
        tempPattern += QDir::separator();
    tempPattern += QLatin1String("XXXXXX.ui");
    QTemporaryFile tempFormFile(tempPattern);

    tempFormFile.setAutoRemove(true);
    if (!tempFormFile.open()) {
        *errorMessage = tr("A temporary form file could not be created in %1.").arg(QDir::tempPath());
        return false;
    }
    const QString tempFormFileName = tempFormFile.fileName();
    tempFormFile.write(contents.toUtf8());
    if (!tempFormFile.flush())  {
        *errorMessage = tr("The temporary form file %1 could not be written.").arg(tempFormFileName);
        return false;
    }
    tempFormFile.close();
    return workbench()->openTemplate(tempFormFileName, m_fileName, errorMessage);

}

QT_END_NAMESPACE
