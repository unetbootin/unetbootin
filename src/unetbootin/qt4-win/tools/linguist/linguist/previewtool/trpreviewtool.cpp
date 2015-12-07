/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QWhatsThis>
#include <QtCore/QIODevice>
#include <QtCore/QPointer>
#include <QtGui/QStandardItemModel>

#include "trpreviewtool.h"
#include "qtwindowlistmenu.h"
#include "messagestreeview.h"

Q_DECLARE_METATYPE(QPointer<FormHolder>)

QT_BEGIN_NAMESPACE

TrPreviewTool::TrPreviewTool(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags), currentTr(0)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui.setupUi(this);
    workspace = new QWorkspace(this);
    setCentralWidget(workspace);

    QtWindowListMenu* wlm = new QtWindowListMenu(workspace,ui.menuBar);
    wlm->addTo(tr("Windows"), ui.menuBar, 2);

    trCombo = new QComboBox(ui.mainToolBar);
    trCombo->setEditable(false);
    trCombo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);
    trCombo->setMinimumContentsLength(16);
    trCombo->addItem(tr("<No Translation>"),QString());

    ui.mainToolBar->insertWidget(ui.actionOpenForm, trCombo);
    ui.mainToolBar->insertSeparator(ui.actionOpenForm);
    QAction* actionWhatsThis = QWhatsThis::createAction(this);
    ui.mainToolBar->addAction(actionWhatsThis);
    ui.menuHelp->insertAction(ui.actionAbout, actionWhatsThis);

    connect(trCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(translationSelected(int)));
    connect(ui.actionOpenForm, SIGNAL(triggered()), this, SLOT(openForm()));
    connect(ui.actionLoadTranslation, SIGNAL(triggered()), this, SLOT(loadTranslation()));
    connect(ui.actionReloadTranslations, SIGNAL(triggered()), this, SLOT(reloadTranslations()));
    connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(showAboutBox()));
    connect(ui.actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui.actionClose, SIGNAL(triggered()), this, SLOT(close()));

    ui.menuViewViews->addAction(ui.dwForms->toggleViewAction());

    m_uiFilesModel = new QStandardItemModel(0, 1, this);
    ui.viewForms->setModel(m_uiFilesModel);
    ui.viewForms->setAlternatingRowColors(true);
    QPalette pal = palette();
    ui.viewForms->setPalette(pal);
}

void TrPreviewTool::on_viewForms_doubleClicked(const QModelIndex &index)
{
    QString path = m_uiFilesModel->data(index, Qt::ToolTipRole).toString();
    QVariant var = m_uiFilesModel->data(index, Qt::UserRole);
    QPointer<FormHolder> holderPtr = qVariantValue<QPointer<FormHolder> >(var);
    if (holderPtr.isNull()) {
        holderPtr = createFormFromFile(path);
        if (holderPtr == 0) {
            showWarning(tr("Could not load form file:\n%1.").arg(path));
            return;
        }
        qVariantSetValue(var, holderPtr);
        m_uiFilesModel->setData(index, var, Qt::UserRole);
    }

    holderPtr->show();
    holderPtr->activateWindow();
    holderPtr->setFocus(Qt::OtherFocusReason);
}

TrPreviewTool::~TrPreviewTool()
{
}

void TrPreviewTool::cascade()
{
    if (workspace) workspace->cascade();
}

void TrPreviewTool::addFormFile(const QString &path)
 {
    QString fileName = QFileInfo(path).fileName();
    if (m_uiFilesModel->findItems(fileName).size() != 0)
        return;

    int row = m_uiFilesModel->rowCount();
    m_uiFilesModel->insertRows(row, 1);
    QModelIndex idx = m_uiFilesModel->index(row, 0);
    m_uiFilesModel->setData(idx, QFileInfo(path).fileName());
    m_uiFilesModel->setData(idx, fileName, Qt::DisplayRole);
    m_uiFilesModel->setData(idx, path, Qt::ToolTipRole);
}

FormHolder* TrPreviewTool::createFormFromFile(const QString& path)
{
    static QStringList formFileList;

    FormHolder* formHolder = new FormHolder(workspace);
    if (!formHolder->loadFormFile(path)) {
            delete formHolder;
            return 0;
    }
    workspace->addWindow(formHolder);
    return formHolder;
}

void TrPreviewTool::openForm()
{
    static QString initDir;
    QStringList pathList = QFileDialog::getOpenFileNames(this,
                                                         tr("Open Forms"),
                                                         initDir,
                                                         tr("User interface form files (*.ui);;All files (*.*)"));
    if (pathList.count())
        initDir = QFileInfo(pathList.first()).absolutePath();
    else
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QHash<QString,FormHolder*> windowDict;
    foreach(QWidget* window,workspace->windowList()) {
        FormHolder* holder = qobject_cast<FormHolder*>(window);
        if (holder)
            windowDict.insert(holder->formFilePath(),holder);
    }
    QString noGoodPaths;
    foreach(const QString &path,pathList) {
        if (windowDict.contains(path)) {
            // Already open
            workspace->setActiveWindow(windowDict.value(path));
        }
        else {
            FormHolder* formHolder = createFormFromFile(path);
            if (!formHolder) {
                noGoodPaths += QDir::toNativeSeparators(path) + QLatin1Char('\n');
            }
            else {
                formHolder->show();
            }
        }
    }

    QApplication::restoreOverrideCursor();

    if (!noGoodPaths.isEmpty())
        showWarning(tr("Could not load form file(s):\n") + noGoodPaths);
}


void TrPreviewTool::recreateForms()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    foreach(QWidget* window,workspace->windowList()) {
            FormHolder* holder = qobject_cast<FormHolder*>(window);
            if (holder)
                holder->retranslate();
    }
    QApplication::restoreOverrideCursor();
}


void TrPreviewTool::translationSelected(int idx)
{
    QTranslator* newTr = trDict.value(trCombo->itemData(idx).toString());
    trCombo->setCurrentIndex(idx);        // If we're called programmatically
    // currentTr out of sync during resulting language change events; fix here if necessary
    if (currentTr)
            QApplication::removeTranslator(currentTr);
    if (newTr)
            QApplication::installTranslator(newTr);
    currentTr = newTr;
    recreateForms();
}

bool TrPreviewTool::loadTranslation(const QString &path, const QString &displayName)
{
    Q_ASSERT(!path.isEmpty());
    QString fn = QFileInfo(path).canonicalFilePath();
    QTranslator* newTr = new QTranslator(this);
    if (!trDict.contains(path)) {
        if (newTr->load(path)) {
                trDict.insert(path, newTr);
            QString trName = displayName.isEmpty() ? QFileInfo(path).fileName() : displayName;
            int idx = trCombo->findText(trName);
            if (idx != -1)
                trName += QString::fromAscii("(%1)").arg(idx);  // Uniqify!
                trCombo->addItem(trName, path);
                trCombo->setCurrentIndex(trCombo->count() - 1);
        } else {
            return false;
        }
    } else {
        //already loaded: make active
        int idx = trCombo->findData(path);
        if (idx >= 0)                        // Should always be true
                translationSelected(idx);
    }
    return true;
}

void TrPreviewTool::addTranslator(QTranslator *translator, const QString &path, const QString &displayName)
{
    if (!trDict.contains(path)) {
        trDict.insert(path, translator);
        QString trName = displayName;
        int idx = trCombo->findText(trName);
        if (idx != -1)
            trName += QString::fromAscii("(%1)").arg(idx);  // Uniqify!
        trCombo->addItem(trName, path);
        trCombo->setCurrentIndex(trCombo->count() - 1);
    } else {
        int idx = trCombo->findData(path);
        if (idx >= 0)                        // Should always be true
            translationSelected(idx);
    }
}

void TrPreviewTool::addTranslator(QTranslator *translator, const QString &displayName)
{
    Q_ASSERT(translator);
    QString path;
    path.sprintf("#:%p", translator);   // the "path" here is a just the string value of the pointer,
                                        // which is always unique, and always start with '#:'.
    addTranslator(translator, path, displayName);
}

void TrPreviewTool::loadTranslation()
{
    //### Handle .ts files as well
    static QString initDir;
    QString path = QFileDialog::getOpenFileName(this,
                                                tr("Load Translation"),
                                                initDir,
                                                tr("Translation files (*.qm);;All files (*.*)"));
    if (!path.isEmpty()) {
            initDir = QFileInfo(path).absolutePath();
        if (!loadTranslation(path)) {
                showWarning(tr("Could not load translation file:\n") + QDir::toNativeSeparators(path));
        }
    }
}

void TrPreviewTool::reloadTranslations()
{
    QString noGoodPaths;
    QList<QTranslator*> oldTrs;
    foreach(const QString &path,trDict.keys()) {
        if (!path.startsWith(QLatin1String("#:"))) {
                QTranslator* newTr = new QTranslator(this); // ### check if we can just reload on the old translator object instead
                if (newTr->load(path)) {
                    oldTrs.append(trDict.value(path));
                    trDict.insert(path, newTr);
                }
                else {
                    noGoodPaths += QDir::toNativeSeparators(path) + QLatin1Char('\n');
                }
        }
    }
    if (!noGoodPaths.isEmpty())
            showWarning(tr("Could not reload translation file(s):\n") + noGoodPaths);
    // Refresh
    translationSelected(trCombo->currentIndex());
    // Clean up now when we are sure it's not in use any longer
    foreach(QTranslator* oldTr,oldTrs) {
            delete oldTr;
    }
}

void TrPreviewTool::showWarning(const QString& warning)
{
    QMessageBox::warning(this, tr("Qt Translation Preview Tool: Warning"),
                         warning,
                         QMessageBox::Ok,
                         QMessageBox::NoButton,
                         QMessageBox::NoButton);
}


void TrPreviewTool::showAboutBox()
{
    QFile f(QString::fromUtf8(":/about.html"));
    f.open(QIODevice::ReadOnly);
    QString aboutText = QString::fromUtf8(f.readAll());

    QMessageBox::about(this, tr("About ") + windowTitle(), aboutText);
}

/**
 * Uninstall the translator if the window was deactivated (i.e. moved to linguist itself)
 * in order to avoid that linguist uses those translations.
 */
bool TrPreviewTool::event(QEvent *e)
{
    if (currentTr) {
        if (e->type() == QEvent::WindowActivate) {
            QApplication::installTranslator(currentTr);
        } else if (e->type() == QEvent::WindowDeactivate) {
            QApplication::removeTranslator(currentTr);
        }
    }
    return QMainWindow::event(e);
}

QUiLoader* FormHolder::uiLoader = 0;

FormHolder::FormHolder(QWidget* parent, Qt::WindowFlags flags)
    : QWidget(parent,flags), form(0)
{
    setAttribute(Qt::WA_DeleteOnClose);
    if (!uiLoader)
        uiLoader = new QUiLoader;
    layout = new QHBoxLayout(this);
    layout->setMargin(0);
    setLayout(layout);
}

QString FormHolder::formFilePath()
{
    return formPath;
}

bool FormHolder::loadFormFile(const QString& path)
{
    formPath = path;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QWidget* newForm = uiLoader->load(&file,this);
    if (!newForm)
        return false;
    delete form;
    form = newForm;

    if (!form->layout()) {
        m_sizeHint = form->sizeHint();
    }else {
        m_sizeHint = form->size();
    }
    form->setWindowFlags(Qt::Widget);
    form->setWindowModality(Qt::NonModal);
    layout->addWidget(form);
    QString ft = QLatin1String(" [") + tr("Preview Form") + QLatin1Char(']');
    ft = form->windowTitle() + ft;
    setWindowTitle(ft);

    return true;
}

QSize FormHolder::sizeHint() const
{
    return m_sizeHint;
}

void FormHolder::retranslate()
{
    loadFormFile(formPath);
}

QT_END_NAMESPACE
