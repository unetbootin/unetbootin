/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

/*  TRANSLATOR MainWindow

  This is the application's main window.
*/

#include "mainwindow.h"

#include "batchtranslationdialog.h"
#include "errorsview.h"
#include "finddialog.h"
#include "formpreviewview.h"
#include "messageeditor.h"
#include "messagemodel.h"
#include "phrasebookbox.h"
#include "phrasemodel.h"
#include "phraseview.h"
#include "printout.h"
#include "sourcecodeview.h"
#include "statistics.h"
#include "translatedialog.h"
#include "translationsettingsdialog.h"

#include <QAction>
#include <QApplication>
#include <QBitmap>
#include <QCloseEvent>
#include <QDebug>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QHeaderView>
#include <QInputDialog>
#include <QItemDelegate>
#include <QLabel>
#include <QLayout>
#include <QLibraryInfo>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPrintDialog>
#include <QProcess>
#include <QRegExp>
#include <QSettings>
#include <QSortFilterProxyModel>
#include <QStackedWidget>
#include <QStatusBar>
#include <QTextStream>
#include <QToolBar>
#include <QUrl>
#include <QWhatsThis>

QT_BEGIN_NAMESPACE

static const int MessageMS = 2500;

const QString &settingsPrefix()
{
    static QString prefix = QString(QLatin1String("%1.%2/"))
        .arg((QT_VERSION >> 16) & 0xff)
        .arg((QT_VERSION >> 8) & 0xff);
    return prefix;
}

enum Ending {
    End_None,
    End_FullStop,
    End_Interrobang,
    End_Colon,
    End_Ellipsis
};

static bool hasFormPreview(const QString &fileName)
{
    return fileName.endsWith(QLatin1String(".ui"))
      || fileName.endsWith(QLatin1String(".jui"));
}

static Ending ending(QString str, QLocale::Language lang)
{
    str = str.simplified();
    int ch = 0;
    if (!str.isEmpty())
        ch = str.right(1)[0].unicode();

    switch (ch) {
    case 0x002e: // full stop
        if (str.endsWith(QString(QLatin1String("..."))))
            return End_Ellipsis;
        else
            return End_FullStop;
    case 0x0589: // armenian full stop
    case 0x06d4: // arabic full stop
    case 0x3002: // ideographic full stop
        return End_FullStop;
    case 0x0021: // exclamation mark
    case 0x003f: // question mark
    case 0x00a1: // inverted exclamation mark
    case 0x00bf: // inverted question mark
    case 0x01c3: // latin letter retroflex click
    case 0x037e: // greek question mark
    case 0x061f: // arabic question mark
    case 0x203c: // double exclamation mark
    case 0x203d: // interrobang
    case 0x2048: // question exclamation mark
    case 0x2049: // exclamation question mark
    case 0x2762: // heavy exclamation mark ornament
        return End_Interrobang;
    case 0x003b: // greek 'compatibility' questionmark
        return lang == QLocale::Greek ? End_Interrobang : End_None;
    case 0x003a: // colon
        return End_Colon;
    case 0x2026: // horizontal ellipsis
        return End_Ellipsis;
    default:
        return End_None;
    }
}


class ContextItemDelegate : public QItemDelegate
{
public:
    ContextItemDelegate(QObject *parent, MultiDataModel *model) : QItemDelegate(parent), m_dataModel(model) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const
    {
        const QAbstractItemModel *model = index.model();
        Q_ASSERT(model);

        if (!model->parent(index).isValid()) {
            if (index.column() - 1 == m_dataModel->modelCount()) {
                QStyleOptionViewItem opt = option;
                opt.font.setBold(true);
                QItemDelegate::paint(painter, opt, index);
                return;
            }
        }
        QItemDelegate::paint(painter, option, index);
    }

private:
    MultiDataModel *m_dataModel;
};

static const QVariant &pxObsolete()
{
    static const QVariant v = 
        qVariantFromValue(QPixmap(QLatin1String(":/images/s_check_obsolete.png")));
    return v;
}


class SortedMessagesModel : public QSortFilterProxyModel
{
public:
    SortedMessagesModel(QObject *parent, MultiDataModel *model) : QSortFilterProxyModel(parent), m_dataModel(model) {}

    QVariant headerData(int section, Qt::Orientation orientation, int role) const
    {
        if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
            switch (section - m_dataModel->modelCount()) {
                case 0: return QString();
                case 1: return MainWindow::tr("Source text");
                case 2: return MainWindow::tr("Index");
            }

        if (role == Qt::DecorationRole && orientation == Qt::Horizontal && section - 1 < m_dataModel->modelCount())
            return pxObsolete();

        return QVariant();
    }

private:
    MultiDataModel *m_dataModel;
};

class SortedContextsModel : public QSortFilterProxyModel
{
public:
    SortedContextsModel(QObject *parent, MultiDataModel *model) : QSortFilterProxyModel(parent), m_dataModel(model) {}

    QVariant headerData(int section, Qt::Orientation orientation, int role) const
    {
        if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
            switch (section - m_dataModel->modelCount()) {
                case 0: return QString();
                case 1: return MainWindow::tr("Context");
                case 2: return MainWindow::tr("Items");
                case 3: return MainWindow::tr("Index");
            }

        if (role == Qt::DecorationRole && orientation == Qt::Horizontal && section - 1 < m_dataModel->modelCount())
            return pxObsolete();

        return QVariant();
    }

private:
    MultiDataModel *m_dataModel;
};

class FocusWatcher : public QObject
{
public:
    FocusWatcher(MessageEditor *msgedit, QObject *parent) : QObject(parent), m_messageEditor(msgedit) {}

protected:
    bool eventFilter(QObject *object, QEvent *event);

private:
    MessageEditor *m_messageEditor;
};

bool FocusWatcher::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::FocusIn)
        m_messageEditor->setEditorFocus(-1);
    return false;
}

MainWindow::MainWindow()
    : QMainWindow(0, Qt::Window),
      m_assistantProcess(0),
      m_findMatchCase(Qt::CaseInsensitive),
      m_findIgnoreAccelerators(true),
      m_findWhere(DataModel::NoLocation),
      m_foundWhere(DataModel::NoLocation),
      m_translationSettingsDialog(0),
      m_settingCurrentMessage(false),
      m_fileActiveModel(-1),
      m_editActiveModel(-1),
      m_statistics(0)
{
    m_ui.setupUi(this);

#ifndef Q_WS_MAC
    setWindowIcon(QPixmap(QLatin1String(":/images/appicon.png") ));
#endif

    m_dataModel = new MultiDataModel(this);
    m_messageModel = new MessageModel(this, m_dataModel);

    // Set up the context dock widget
    m_contextDock = new QDockWidget(this);
    m_contextDock->setObjectName(QLatin1String("ContextDockWidget"));
    m_contextDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_contextDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_contextDock->setWindowTitle(tr("Context"));
    m_contextDock->setAcceptDrops(true);
    m_contextDock->installEventFilter(this);

    m_sortedContextsModel = new SortedContextsModel(this, m_dataModel);
    m_sortedContextsModel->setSortRole(MessageModel::SortRole);
    m_sortedContextsModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortedContextsModel->setSourceModel(m_messageModel);

    m_contextView = new QTreeView(this);
    m_contextView->setRootIsDecorated(false);
    m_contextView->setItemsExpandable(false);
    m_contextView->setUniformRowHeights(true);
    m_contextView->setAlternatingRowColors(true);
    m_contextView->setAllColumnsShowFocus(true);
    m_contextView->setItemDelegate(new ContextItemDelegate(this, m_dataModel));
    m_contextView->setSortingEnabled(true);
    m_contextView->setWhatsThis(tr("This panel lists the source contexts."));
    m_contextView->setModel(m_sortedContextsModel);
    m_contextView->header()->setMovable(false);
    m_contextView->setColumnHidden(0, true);
    m_contextView->header()->setResizeMode(1, QHeaderView::Stretch);
    m_contextView->header()->setResizeMode(2, QHeaderView::ResizeToContents);
    m_contextView->header()->setStretchLastSection(false);

    m_contextDock->setWidget(m_contextView);

    // Set up the messages dock widget
    m_messagesDock = new QDockWidget(this);
    m_messagesDock->setObjectName(QLatin1String("StringsDockWidget"));
    m_messagesDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_messagesDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_messagesDock->setWindowTitle(tr("Strings"));
    m_messagesDock->setAcceptDrops(true);
    m_messagesDock->installEventFilter(this);

    m_sortedMessagesModel = new SortedMessagesModel(this, m_dataModel);
    m_sortedMessagesModel->setSortRole(MessageModel::SortRole);
    m_sortedMessagesModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortedMessagesModel->setSortLocaleAware(true);
    m_sortedMessagesModel->setSourceModel(m_messageModel);

    m_messageView = new QTreeView(m_messagesDock);
    m_messageView->setSortingEnabled(true);
    m_messageView->setRootIsDecorated(false);
    m_messageView->setUniformRowHeights(true);
    m_messageView->setAllColumnsShowFocus(true);
    m_messageView->setItemsExpandable(false);
    m_messageView->setModel(m_sortedMessagesModel);
    m_messageView->header()->setMovable(false);
    m_messageView->setColumnHidden(0, true);
    m_messageView->setColumnHidden(2, true);
    // last visible column auto-stretches

    m_messagesDock->setWidget(m_messageView);

    // Set up main message view
    m_messageEditor = new MessageEditor(m_dataModel, this);
    m_messageEditor->setAcceptDrops(true);
    m_messageEditor->installEventFilter(this);
    // We can't call setCentralWidget(m_messageEditor), since it is already called in m_ui.setupUi()
    QBoxLayout *lout = new QBoxLayout(QBoxLayout::TopToBottom, m_ui.centralwidget);
    lout->addWidget(m_messageEditor);
    lout->setMargin(0);
    m_ui.centralwidget->setLayout(lout);

    // Set up the phrases & guesses dock widget
    m_phrasesDock = new QDockWidget(this);
    m_phrasesDock->setObjectName(QLatin1String("PhrasesDockwidget"));
    m_phrasesDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_phrasesDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_phrasesDock->setWindowTitle(tr("Phrases and guesses"));

    m_phraseView = new PhraseView(m_dataModel, &m_phraseDict, this);
    m_phrasesDock->setWidget(m_phraseView);

    // Set up source code and form preview dock widget
    m_sourceAndFormDock = new QDockWidget(this);
    m_sourceAndFormDock->setObjectName(QLatin1String("SourceAndFormDock"));
    m_sourceAndFormDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_sourceAndFormDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_sourceAndFormDock->setWindowTitle(tr("Sources and Forms"));
    m_sourceAndFormView = new QStackedWidget(this);
    m_sourceAndFormDock->setWidget(m_sourceAndFormView);
    //connect(m_sourceAndDock, SIGNAL(visibilityChanged(bool)),
    //    m_sourceCodeView, SLOT(setActivated(bool)));
    m_formPreviewView = new FormPreviewView(0, m_dataModel);
    m_sourceCodeView = new SourceCodeView(0);
    m_sourceAndFormView->addWidget(m_sourceCodeView);
    m_sourceAndFormView->addWidget(m_formPreviewView);

    // Set up errors dock widget
    m_errorsDock = new QDockWidget(this);
    m_errorsDock->setObjectName(QLatin1String("ErrorsDockWidget"));
    m_errorsDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_errorsDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_errorsDock->setWindowTitle(tr("Warnings"));
    m_errorsView = new ErrorsView(m_dataModel, this);
    m_errorsDock->setWidget(m_errorsView);

    // Arrange dock widgets
    setDockNestingEnabled(true);
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, m_contextDock);
    addDockWidget(Qt::TopDockWidgetArea, m_messagesDock);
    addDockWidget(Qt::BottomDockWidgetArea, m_phrasesDock);
    addDockWidget(Qt::TopDockWidgetArea, m_sourceAndFormDock);
    addDockWidget(Qt::BottomDockWidgetArea, m_errorsDock);
    //tabifyDockWidget(m_errorsDock, m_sourceAndFormDock);
    //tabifyDockWidget(m_sourceCodeDock, m_phrasesDock);

    // Allow phrases doc to intercept guesses shortcuts
    m_messageEditor->installEventFilter(m_phraseView);

    // Set up shortcuts for the dock widgets
    QShortcut *contextShortcut = new QShortcut(QKeySequence(Qt::Key_F6), this);
    connect(contextShortcut, SIGNAL(activated()), this, SLOT(showContextDock()));
    QShortcut *messagesShortcut = new QShortcut(QKeySequence(Qt::Key_F7), this);
    connect(messagesShortcut, SIGNAL(activated()), this, SLOT(showMessagesDock()));
    QShortcut *errorsShortcut = new QShortcut(QKeySequence(Qt::Key_F8), this);
    connect(errorsShortcut, SIGNAL(activated()), this, SLOT(showErrorDock()));
    QShortcut *sourceCodeShortcut = new QShortcut(QKeySequence(Qt::Key_F9), this);
    connect(sourceCodeShortcut, SIGNAL(activated()), this, SLOT(showSourceCodeDock()));
    QShortcut *phrasesShortcut = new QShortcut(QKeySequence(Qt::Key_F10), this);
    connect(phrasesShortcut, SIGNAL(activated()), this, SLOT(showPhrasesDock()));

    connect(m_phraseView, SIGNAL(phraseSelected(int,QString)),
            m_messageEditor, SLOT(setTranslation(int,QString)));
    connect(m_contextView->selectionModel(),
            SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(selectedContextChanged(QModelIndex,QModelIndex)));
    connect(m_messageView->selectionModel(),
            SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(selectedMessageChanged(QModelIndex,QModelIndex)));
    connect(m_contextView->selectionModel(),
            SIGNAL(currentColumnChanged(QModelIndex,QModelIndex)),
            SLOT(updateLatestModel(QModelIndex)));
    connect(m_messageView->selectionModel(),
            SIGNAL(currentColumnChanged(QModelIndex,QModelIndex)),
            SLOT(updateLatestModel(QModelIndex)));

    connect(m_messageEditor, SIGNAL(activeModelChanged(int)), SLOT(updateActiveModel(int)));

    m_translateDialog = new TranslateDialog(this);
    m_batchTranslateDialog = new BatchTranslationDialog(m_dataModel, this);
    m_findDialog = new FindDialog(this);

    setupMenuBar();
    setupToolBars();

    m_progressLabel = new QLabel();
    statusBar()->addPermanentWidget(m_progressLabel);
    m_modifiedLabel = new QLabel(tr(" MOD ", "status bar: file(s) modified"));
    statusBar()->addPermanentWidget(m_modifiedLabel);

    modelCountChanged();
    resetSorting();

    connect(m_dataModel, SIGNAL(modifiedChanged(bool)),
            this, SLOT(setWindowModified(bool)));
    connect(m_dataModel, SIGNAL(modifiedChanged(bool)),
            m_modifiedLabel, SLOT(setVisible(bool)));
    connect(m_dataModel, SIGNAL(multiContextDataChanged(MultiDataIndex)),
            SLOT(updateProgress()));
    connect(m_dataModel, SIGNAL(messageDataChanged(MultiDataIndex)),
            SLOT(maybeUpdateStatistics(MultiDataIndex)));
    connect(m_dataModel, SIGNAL(translationChanged(MultiDataIndex)),
            SLOT(translationChanged(MultiDataIndex)));
    connect(m_dataModel, SIGNAL(languageChanged(int)),
            SLOT(updatePhraseDict(int)));

    setWindowModified(m_dataModel->isModified());
    m_modifiedLabel->setVisible(m_dataModel->isModified());

    connect(m_messageView, SIGNAL(clicked(QModelIndex)),
            this, SLOT(toggleFinished(QModelIndex)));
    connect(m_messageView, SIGNAL(activated(QModelIndex)),
            m_messageEditor, SLOT(setEditorFocus()));
    connect(m_contextView, SIGNAL(activated(QModelIndex)),
            m_messageView, SLOT(setFocus()));
    connect(m_messageEditor, SIGNAL(translationChanged(QStringList)),
            this, SLOT(updateTranslation(QStringList)));
    connect(m_messageEditor, SIGNAL(translatorCommentChanged(QString)),
            this, SLOT(updateTranslatorComment(QString)));
    connect(m_findDialog, SIGNAL(findNext(QString,DataModel::FindLocation,bool,bool)),
            this, SLOT(findNext(QString,DataModel::FindLocation,bool,bool)));
    connect(m_translateDialog, SIGNAL(requestMatchUpdate(bool &)), SLOT(updateTranslateHit(bool &)));
    connect(m_translateDialog, SIGNAL(activated(int)), SLOT(translate(int)));

    QSize as(qApp->desktop()->size());
    as -= QSize(30, 30);
    resize(QSize(1000, 800).boundedTo(as));
    show();
    readConfig();
    m_statistics = 0;

    m_focusWatcher = new FocusWatcher(m_messageEditor, this);
    m_contextView->installEventFilter(m_focusWatcher);
    m_messageView->installEventFilter(m_focusWatcher);
    m_messageEditor->installEventFilter(m_focusWatcher);
    m_sourceAndFormView->installEventFilter(m_focusWatcher);
    m_phraseView->installEventFilter(m_focusWatcher);
    m_errorsView->installEventFilter(m_focusWatcher);
}

MainWindow::~MainWindow()
{
    writeConfig();
    if (m_assistantProcess && m_assistantProcess->state() == QProcess::Running) {
        m_assistantProcess->terminate();
        m_assistantProcess->waitForFinished(3000);
    }
    qDeleteAll(m_phraseBooks);
    delete m_dataModel;
    delete m_statistics;
}

void MainWindow::modelCountChanged()
{
    int mc = m_dataModel->modelCount();

    for (int i = 0; i < mc; ++i) {
        m_contextView->header()->setResizeMode(i + 1, QHeaderView::Fixed);
        m_contextView->header()->resizeSection(i + 1, 24);

        m_messageView->header()->setResizeMode(i + 1, QHeaderView::Fixed);
        m_messageView->header()->resizeSection(i + 1, 24);
    }

    if (!mc) {
        selectedMessageChanged(QModelIndex(), QModelIndex());
        updateLatestModel(-1);
    } else {
        if (!m_contextView->currentIndex().isValid()) {
            // Ensure that something is selected
            m_contextView->setCurrentIndex(m_sortedContextsModel->index(0, 0));
        } else {
            // Plug holes that turn up in the selection due to inserting columns
            m_contextView->selectionModel()->select(m_contextView->currentIndex(),
                        QItemSelectionModel::SelectCurrent|QItemSelectionModel::Rows);
            m_messageView->selectionModel()->select(m_messageView->currentIndex(),
                        QItemSelectionModel::SelectCurrent|QItemSelectionModel::Rows);
        }
        // Field insertions/removals are automatic, but not the re-fill
        m_messageEditor->showMessage(m_currentIndex);
        if (mc == 1)
            updateLatestModel(0);
        else if (m_currentIndex.model() >= mc)
            updateLatestModel(mc - 1);
    }

    m_contextView->setUpdatesEnabled(true);
    m_messageView->setUpdatesEnabled(true);

    updateProgress();
    updateCaption();

    m_ui.actionFind->setEnabled(m_dataModel->contextCount() > 0);
    m_ui.actionFindNext->setEnabled(false);

    m_formPreviewView->setSourceContext(-1, 0);
}

struct OpenedFile {
    OpenedFile(DataModel *_dataModel, bool _readWrite, bool _langGuessed)
        { dataModel = _dataModel; readWrite = _readWrite; langGuessed = _langGuessed; }
    DataModel *dataModel;
    bool readWrite;
    bool langGuessed;
};

bool MainWindow::openFiles(const QStringList &names, bool globalReadWrite)
{
    if (names.isEmpty())
        return false;

    bool waitCursor = false;
    statusBar()->showMessage(tr("Loading..."));
    qApp->processEvents();

    QList<OpenedFile> opened;
    bool closeOld = false;
    foreach (QString name, names) {
        if (!waitCursor) {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            waitCursor = true;
        }

        bool readWrite = globalReadWrite;
        if (name.startsWith(QLatin1Char('='))) {
            name.remove(0, 1);
            readWrite = false;
        }
        QFileInfo fi(name);
        if (fi.exists()) // Make the loader error out instead of reading stdin
            name = fi.canonicalFilePath();
        if (m_dataModel->isFileLoaded(name) >= 0)
            continue;

        bool langGuessed;
        DataModel *dm = new DataModel(m_dataModel);
        if (!dm->load(name, &langGuessed, this)) {
            delete dm;
            continue;
        }
        if (opened.isEmpty()) {
            if (!m_dataModel->isWellMergeable(dm)) {
                QApplication::restoreOverrideCursor();
                waitCursor = false;
                switch (QMessageBox::information(this, tr("Loading File - Qt Linguist"),
                    tr("The file '%1' does not seem to be related to the currently open file(s) '%2'.\n\n"
                       "Close the open file(s) first?")
                       .arg(DataModel::prettifyPlainFileName(name), m_dataModel->condensedSrcFileNames(true)),
                    QMessageBox::Yes | QMessageBox::Default,
                    QMessageBox::No,
                    QMessageBox::Cancel | QMessageBox::Escape))
                {
                    case QMessageBox::Cancel:
                        delete dm;
                        return false;
                    case QMessageBox::Yes:
                        closeOld = true;
                        break;
                    case QMessageBox::No:
                        break;
                }
            }
        } else {
            if (!opened.first().dataModel->isWellMergeable(dm)) {
                QApplication::restoreOverrideCursor();
                waitCursor = false;
                switch (QMessageBox::information(this, tr("Loading File - Qt Linguist"),
                    tr("The file '%1' does not seem to be related to the file '%2'"
                       " which is being loaded as well.\n\n"
                       "Skip loading the first named file?")
                       .arg(DataModel::prettifyPlainFileName(name), opened.first().dataModel->srcFileName(true)),
                    QMessageBox::Yes | QMessageBox::Default,
                    QMessageBox::No,
                    QMessageBox::Cancel | QMessageBox::Escape))
                {
                    case QMessageBox::Cancel:
                        delete dm;
                        foreach (const OpenedFile &op, opened)
                            delete op.dataModel;
                        return false;
                    case QMessageBox::Yes:
                        delete dm;
                        continue;
                    case QMessageBox::No:
                        break;
                }
            }
        }
        opened.append(OpenedFile(dm, readWrite, langGuessed));
    }

    if (closeOld) {
        if (waitCursor) {
            QApplication::restoreOverrideCursor();
            waitCursor = false;
        }
        if (!closeAll()) {
            foreach (const OpenedFile &op, opened)
                delete op.dataModel;
            return false;
        }
    }

    foreach (const OpenedFile &op, opened) {
        if (op.langGuessed) {
            if (waitCursor) {
                QApplication::restoreOverrideCursor();
                waitCursor = false;
            }
            if (!m_translationSettingsDialog)
                m_translationSettingsDialog = new TranslationSettingsDialog(this);
            m_translationSettingsDialog->setDataModel(op.dataModel);
            m_translationSettingsDialog->exec();
        }
    }

    if (!waitCursor)
        QApplication::setOverrideCursor(Qt::WaitCursor);
    m_contextView->setUpdatesEnabled(false);
    m_messageView->setUpdatesEnabled(false);
    int totalCount = 0;
    foreach (const OpenedFile &op, opened) {
        m_phraseDict.append(QHash<QString, QList<Phrase *> >());
        m_dataModel->append(op.dataModel, op.readWrite);
        if (op.readWrite)
            updatePhraseDictInternal(m_phraseDict.size() - 1);
        totalCount += op.dataModel->messageCount();
    }
    statusBar()->showMessage(tr("%n translation unit(s) loaded.", 0, totalCount), MessageMS);
    modelCountChanged();
    recentFiles().addFiles(m_dataModel->srcFileNames());

    revalidate();
    QApplication::restoreOverrideCursor();
    return true;
}

RecentFiles &MainWindow::recentFiles()
{
    static RecentFiles recentFiles(10);
    return recentFiles;
}

const QString &MainWindow::resourcePrefix()
{
#ifdef Q_WS_MAC
    static const QString prefix(QLatin1String(":/images/mac"));
#else
    static const QString prefix(QLatin1String(":/images/win"));
#endif
    return prefix;
}

void MainWindow::open()
{
    openFiles(pickTranslationFiles());
}

void MainWindow::openAux()
{
    openFiles(pickTranslationFiles(), false);
}

void MainWindow::closeFile()
{
    int model = m_currentIndex.model();
    if (model >= 0 && maybeSave(model)) {
        m_phraseDict.removeAt(model);
        m_contextView->setUpdatesEnabled(false);
        m_messageView->setUpdatesEnabled(false);
        m_dataModel->close(model);
        modelCountChanged();
    }
}

bool MainWindow::closeAll()
{
    if (maybeSaveAll()) {
        m_phraseDict.clear();
        m_contextView->setUpdatesEnabled(false);
        m_messageView->setUpdatesEnabled(false);
        m_dataModel->closeAll();
        modelCountChanged();
        recentFiles().closeGroup();
        return true;
    }
    return false;
}

static QString fileFilters(bool allFirst)
{
    static const QString pattern(QLatin1String("%1 (*.%2);;"));
    QStringList allExtensions;
    QString filter;
    foreach (const Translator::FileFormat &format, Translator::registeredFileFormats()) {
        if (format.fileType == Translator::FileFormat::TranslationSource && format.priority >= 0) {
            filter.append(pattern.arg(format.description).arg(format.extension));
            allExtensions.append(QLatin1String("*.") + format.extension);
        }
    }
    QString allFilter = QObject::tr("Translation files (%1);;").arg(allExtensions.join(QLatin1String(" ")));
    if (allFirst)
        filter.prepend(allFilter);
    else
        filter.append(allFilter);
    filter.append(QObject::tr("All files (*)"));
    return filter;
}

QStringList MainWindow::pickTranslationFiles()
{
    QString dir;
    if (!recentFiles().isEmpty())
        dir = QFileInfo(recentFiles().lastOpenedFile()).path();

    QString varFilt;
    if (m_dataModel->modelCount()) {
        QFileInfo mainFile(m_dataModel->srcFileName(0));
        QString mainFileBase = mainFile.baseName();
        int pos = mainFileBase.indexOf(QLatin1Char('_'));
        if (pos > 0)
            varFilt = tr("Related files (%1);;")
                .arg(mainFileBase.left(pos) + QLatin1String("_*.") + mainFile.completeSuffix());
    }

    return QFileDialog::getOpenFileNames(this, tr("Open Translation Files"), dir,
        varFilt +
        fileFilters(true));
}

void MainWindow::saveInternal(int model)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (m_dataModel->save(model, this)) {
        updateCaption();
        statusBar()->showMessage(tr("File saved."), MessageMS);
    }
    QApplication::restoreOverrideCursor();
}

void MainWindow::saveAll()
{
    for (int i = 0; i < m_dataModel->modelCount(); ++i)
        if (m_dataModel->isModelWritable(i))
            saveInternal(i);
    recentFiles().closeGroup();
}

void MainWindow::save()
{
    if (m_currentIndex.model() < 0)
        return;

    saveInternal(m_currentIndex.model());
}

void MainWindow::saveAs()
{
    if (m_currentIndex.model() < 0)
        return;

    QString newFilename = QFileDialog::getSaveFileName(this, QString(), m_dataModel->srcFileName(m_currentIndex.model()),
        fileFilters(false));
    if (!newFilename.isEmpty()) {
        if (m_dataModel->saveAs(m_currentIndex.model(), newFilename, this)) {
            updateCaption();
            statusBar()->showMessage(tr("File saved."), MessageMS);
            recentFiles().addFiles(m_dataModel->srcFileNames());
        }
    }
}

void MainWindow::releaseAs()
{
    if (m_currentIndex.model() < 0)
        return;

    QFileInfo oldFile(m_dataModel->srcFileName(m_currentIndex.model()));
    QString newFilename = oldFile.path() + QLatin1String("/")
                + oldFile.completeBaseName() + QLatin1String(".qm");

    newFilename = QFileDialog::getSaveFileName(this, tr("Release"), newFilename,
        tr("Qt message files for released applications (*.qm)\nAll files (*)"));
    if (!newFilename.isEmpty()) {
        if (m_dataModel->release(m_currentIndex.model(), newFilename, false, false, SaveEverything, this))
            statusBar()->showMessage(tr("File created."), MessageMS);
    }
}

void MainWindow::releaseInternal(int model)
{
    QFileInfo oldFile(m_dataModel->srcFileName(model));
    QString newFilename = oldFile.path() + QLatin1Char('/')
                + oldFile.completeBaseName() + QLatin1String(".qm");

    if (!newFilename.isEmpty()) {
        if (m_dataModel->release(model, newFilename, false, false, SaveEverything, this))
            statusBar()->showMessage(tr("File created."), MessageMS);
    }
}

// No-question
void MainWindow::release()
{
    if (m_currentIndex.model() < 0)
        return;

    releaseInternal(m_currentIndex.model());
}

void MainWindow::releaseAll()
{
    for (int i = 0; i < m_dataModel->modelCount(); ++i)
        if (m_dataModel->isModelWritable(i))
            releaseInternal(i);
}

void MainWindow::print()
{
    int pageNum = 0;
    QPrintDialog dlg(&m_printer, this);
    if (dlg.exec()) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        m_printer.setDocName(m_dataModel->condensedSrcFileNames(true));
        statusBar()->showMessage(tr("Printing..."));
        PrintOut pout(&m_printer);

        for (int i = 0; i < m_dataModel->contextCount(); ++i) {
            MultiContextItem *mc = m_dataModel->multiContextItem(i);
            pout.vskip();
            pout.setRule(PrintOut::ThickRule);
            pout.setGuide(mc->context());
            pout.addBox(100, tr("Context: %1").arg(mc->context()),
                PrintOut::Strong);
            pout.flushLine();
            pout.addBox(4);
            pout.addBox(92, mc->comment(), PrintOut::Emphasis);
            pout.flushLine();
            pout.setRule(PrintOut::ThickRule);

            for (int j = 0; j < mc->messageCount(); ++j) {
                pout.setRule(PrintOut::ThinRule);
                bool printedSrc = false;
                QString comment;
                for (int k = 0; k < m_dataModel->modelCount(); ++k) {
                    if (const MessageItem *m = mc->messageItem(k, j)) {
                        if (!printedSrc) {
                            pout.addBox(40, m->text());
                            pout.addBox(4);
                            comment = m->comment();
                            printedSrc = true;
                        } else {
                            pout.addBox(44); // Maybe put the name of the translation here
                        }
                        if (m->message().isPlural() && m_dataModel->language(k) != QLocale::C) {
                            QStringList transls = m->translations();
                            pout.addBox(40, transls.join(QLatin1String("\n")));
                        } else {
                            pout.addBox(40, m->translation());
                        }
                        pout.addBox(4);
                        QString type;
                        switch (m->message().type()) {
                        case TranslatorMessage::Finished:
                            type = tr("finished");
                            break;
                        case TranslatorMessage::Unfinished:
                            type = m->danger() ? tr("unresolved") : QLatin1String("unfinished");
                            break;
                        case TranslatorMessage::Obsolete:
                            type = tr("obsolete");
                            break;
                        }
                        pout.addBox(12, type, PrintOut::Normal, Qt::AlignRight);
                        pout.flushLine();
                    }
                }
                if (!comment.isEmpty()) {
                    pout.addBox(4);
                    pout.addBox(92, comment, PrintOut::Emphasis);
                    pout.flushLine(true);
                }

                if (pout.pageNum() != pageNum) {
                    pageNum = pout.pageNum();
                    statusBar()->showMessage(tr("Printing... (page %1)")
                        .arg(pageNum));
                }
            }
        }
        pout.flushLine(true);
        QApplication::restoreOverrideCursor();
        statusBar()->showMessage(tr("Printing completed"), MessageMS);
    } else {
        statusBar()->showMessage(tr("Printing aborted"), MessageMS);
    }
}

bool MainWindow::searchItem(const QString &searchWhat)
{
    if ((m_findWhere & m_foundWhere) == 0)
        return false;

    QString text = searchWhat;

    if (m_findIgnoreAccelerators)
        // FIXME: This removes too much. The proper solution might be too slow, though.
        text.remove(QLatin1Char('&'));

    int foundOffset = text.indexOf(m_findText, 0, m_findMatchCase);
    return foundOffset >= 0;
}

void MainWindow::findAgain()
{
    if (m_dataModel->contextCount() == 0)
        return;

    const QModelIndex &startIndex = m_messageView->currentIndex();
    QModelIndex index = nextMessage(startIndex);

    while (index.isValid()) {
        QModelIndex realIndex = m_sortedMessagesModel->mapToSource(index);
        MultiDataIndex dataIndex = m_messageModel->dataIndex(realIndex, -1);
        bool hadMessage = false;
        for (int i = 0; i < m_dataModel->modelCount(); ++i) {
            if (MessageItem *m = m_dataModel->messageItem(dataIndex, i)) {
                // Note: we do not look into plurals on grounds of them not
                // containing anything much different from the singular.
                if (hadMessage) {
                    m_foundWhere = DataModel::Translations;
                    if (!searchItem(m->translation()))
                        m_foundWhere = DataModel::NoLocation;
                } else {
                    switch (m_foundWhere) {
                    case 0:
                        m_foundWhere = DataModel::SourceText;
                        // fall-through to search source text
                    case DataModel::SourceText:
                        if (searchItem(m->text()))
                            break;
                        if (searchItem(m->pluralText()))
                            break;
                        m_foundWhere = DataModel::Translations;
                        // fall-through to search translation
                    case DataModel::Translations:
                        if (searchItem(m->translation()))
                            break;
                        m_foundWhere = DataModel::Comments;
                        // fall-through to search comment
                    case DataModel::Comments:
                        if (searchItem(m->comment()))
                            break;
                        if (searchItem(m->extraComment()))
                            break;
                        m_foundWhere = DataModel::NoLocation;
                        // did not find the search string in this message
                    }
                }
                if (m_foundWhere != DataModel::NoLocation) {
                    setCurrentMessage(realIndex, i);

                    // determine whether the search wrapped
                    const QModelIndex &c1 = m_sortedContextsModel->mapFromSource(
                            m_sortedMessagesModel->mapToSource(startIndex)).parent();
                    const QModelIndex &c2 = m_sortedContextsModel->mapFromSource(realIndex).parent();
                    const QModelIndex &m = m_sortedMessagesModel->mapFromSource(realIndex);

                    if (c2.row() < c1.row() || (c1.row() == c2.row() && m.row() <= startIndex.row()))
                        statusBar()->showMessage(tr("Search wrapped."), MessageMS);

                    m_findDialog->hide();
                    return;
                }
                hadMessage = true;
            }
        }

        // since we don't search startIndex at the beginning, only now we have searched everything
        if (index == startIndex)
            break;

        index = nextMessage(index);
    }

    qApp->beep();
    QMessageBox::warning(m_findDialog, tr("Qt Linguist"),
                         tr("Cannot find the string '%1'.").arg(m_findText));
    m_foundWhere  = DataModel::NoLocation;
}

void MainWindow::showBatchTranslateDialog()
{
    m_messageModel->blockSignals(true);
    m_batchTranslateDialog->setPhraseBooks(m_phraseBooks, m_currentIndex.model());
    if (m_batchTranslateDialog->exec() != QDialog::Accepted)
        m_messageModel->blockSignals(false);
    // else signal finished() calls refreshItemViews()
}

void MainWindow::showTranslateDialog()
{
    m_latestCaseSensitivity = -1;
    QModelIndex idx = m_messageView->currentIndex();
    QModelIndex idx2 = m_sortedMessagesModel->index(idx.row(), m_currentIndex.model() + 1, idx.parent());
    m_messageView->setCurrentIndex(idx2);
    QString fn = QFileInfo(m_dataModel->srcFileName(m_currentIndex.model())).baseName();
    m_translateDialog->setWindowTitle(tr("Search And Translate in '%1' - Qt Linguist").arg(fn));
    m_translateDialog->exec();
}

void MainWindow::updateTranslateHit(bool &hit)
{
    MessageItem *m;
    hit = (m = m_dataModel->messageItem(m_currentIndex))
          && !m->isObsolete()
          && m->compare(m_translateDialog->findText(), false, m_translateDialog->caseSensitivity());
}

void MainWindow::translate(int mode)
{
    QString findText = m_translateDialog->findText();
    QString replaceText = m_translateDialog->replaceText();
    bool markFinished = m_translateDialog->markFinished();
    Qt::CaseSensitivity caseSensitivity = m_translateDialog->caseSensitivity();

    int translatedCount = 0;

    if (mode == TranslateDialog::TranslateAll) {
        for (MultiDataModelIterator it(m_dataModel, m_currentIndex.model()); it.isValid(); ++it) {
            MessageItem *m = it.current();
            if (m && !m->isObsolete() && m->compare(findText, false, caseSensitivity)) {
                if (!translatedCount)
                    m_messageModel->blockSignals(true);
                m_dataModel->setTranslation(it, replaceText);
                m_dataModel->setFinished(it, markFinished);
                ++translatedCount;
            }
        }
        if (translatedCount) {
            refreshItemViews();
            QMessageBox::warning(m_translateDialog, tr("Translate - Qt Linguist"),
                    tr("Translated %n entry(s)", 0, translatedCount));
        }
    } else {
        if (mode == TranslateDialog::Translate) {
            m_dataModel->setTranslation(m_currentIndex, replaceText);
            m_dataModel->setFinished(m_currentIndex, markFinished);
        }

        if (findText != m_latestFindText || caseSensitivity != m_latestCaseSensitivity) {
            m_latestFindText = findText;
            m_latestCaseSensitivity = caseSensitivity;
            m_remainingCount = m_dataModel->messageCount();
            m_hitCount = 0;
        }

        QModelIndex index = m_messageView->currentIndex();
        int prevRemained = m_remainingCount;
        forever {
            if (--m_remainingCount <= 0) {
                if (!m_hitCount)
                    break;
                m_remainingCount = m_dataModel->messageCount() - 1;
                if (QMessageBox::question(m_translateDialog, tr("Translate - Qt Linguist"),
                        tr("No more occurrences of '%1'. Start over?").arg(findText),
                        QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes)
                    return;
                m_remainingCount -= prevRemained;
            }

            index = nextMessage(index);

            QModelIndex realIndex = m_sortedMessagesModel->mapToSource(index);
            MultiDataIndex dataIndex = m_messageModel->dataIndex(realIndex, m_currentIndex.model());
            if (MessageItem *m = m_dataModel->messageItem(dataIndex)) {
                if (!m->isObsolete() && m->compare(findText, false, caseSensitivity)) {
                    setCurrentMessage(realIndex, m_currentIndex.model());
                    ++translatedCount;
                    ++m_hitCount;
                    break;
                }
            }
        }
    }

    if (!translatedCount) {
        qApp->beep();
        QMessageBox::warning(m_translateDialog, tr("Translate - Qt Linguist"),
                tr("Cannot find the string '%1'.").arg(findText));
    }
}

void MainWindow::newPhraseBook()
{
    QString name = QFileDialog::getSaveFileName(this, tr("Create New Phrase Book"),
            m_phraseBookDir, tr("Qt phrase books (*.qph)\nAll files (*)"));
    if (!name.isEmpty()) {
        PhraseBook pb;
        if (!m_translationSettingsDialog)
            m_translationSettingsDialog = new TranslationSettingsDialog(this);
        m_translationSettingsDialog->setPhraseBook(&pb);
        if (!m_translationSettingsDialog->exec())
            return;
        m_phraseBookDir = QFileInfo(name).absolutePath();
        if (savePhraseBook(&name, pb)) {
            if (openPhraseBook(name))
                statusBar()->showMessage(tr("Phrase book created."), MessageMS);
        }
    }
}

bool MainWindow::isPhraseBookOpen(const QString &name)
{
    foreach(const PhraseBook *pb, m_phraseBooks) {
        if (pb->fileName() == name)
            return true;
    }

    return false;
}

void MainWindow::openPhraseBook()
{
    QString name = QFileDialog::getOpenFileName(this, tr("Open Phrase Book"),
    m_phraseBookDir, tr("Qt phrase books (*.qph);;All files (*)"));

    if (!name.isEmpty()) {
        m_phraseBookDir = QFileInfo(name).absolutePath();
        if (!isPhraseBookOpen(name)) {
            if (PhraseBook *phraseBook = openPhraseBook(name)) {
                int n = phraseBook->phrases().count();
                statusBar()->showMessage(tr("%n phrase(s) loaded.", 0, n), MessageMS);
            }
        }
    }
}

void MainWindow::closePhraseBook(QAction *action)
{
    PhraseBook *pb = m_phraseBookMenu[PhraseCloseMenu].value(action);
    if (!maybeSavePhraseBook(pb))
        return;

    m_phraseBookMenu[PhraseCloseMenu].remove(action);
    m_ui.menuClosePhraseBook->removeAction(action);

    QAction *act = m_phraseBookMenu[PhraseEditMenu].key(pb);
    m_phraseBookMenu[PhraseEditMenu].remove(act);
    m_ui.menuEditPhraseBook->removeAction(act);

    act = m_phraseBookMenu[PhrasePrintMenu].key(pb);
    m_ui.menuPrintPhraseBook->removeAction(act);

    m_phraseBooks.removeOne(pb);
    disconnect(pb, SIGNAL(listChanged()), this, SLOT(updatePhraseDicts()));
    updatePhraseDicts();
    delete pb;
    updatePhraseBookActions();
}

void MainWindow::editPhraseBook(QAction *action)
{
    PhraseBook *pb = m_phraseBookMenu[PhraseEditMenu].value(action);
    PhraseBookBox box(pb, this);
    box.exec();

    updatePhraseDicts();
}

void MainWindow::printPhraseBook(QAction *action)
{
    PhraseBook *phraseBook = m_phraseBookMenu[PhrasePrintMenu].value(action);

    int pageNum = 0;

    QPrintDialog dlg(&m_printer, this);
    if (dlg.exec()) {
        m_printer.setDocName(phraseBook->fileName());
        statusBar()->showMessage(tr("Printing..."));
        PrintOut pout(&m_printer);
        pout.setRule(PrintOut::ThinRule);
        foreach (const Phrase *p, phraseBook->phrases()) {
            pout.setGuide(p->source());
            pout.addBox(29, p->source());
            pout.addBox(4);
            pout.addBox(29, p->target());
            pout.addBox(4);
            pout.addBox(34, p->definition(), PrintOut::Emphasis);

            if (pout.pageNum() != pageNum) {
                pageNum = pout.pageNum();
                statusBar()->showMessage(tr("Printing... (page %1)")
                    .arg(pageNum));
            }
            pout.setRule(PrintOut::NoRule);
            pout.flushLine(true);
        }
        pout.flushLine(true);
        statusBar()->showMessage(tr("Printing completed"), MessageMS);
    } else {
        statusBar()->showMessage(tr("Printing aborted"), MessageMS);
    }
}

void MainWindow::addToPhraseBook()
{
    MessageItem *currentMessage = m_dataModel->messageItem(m_currentIndex);
    Phrase *phrase = new Phrase(currentMessage->text(), currentMessage->translation(), QString());
    QStringList phraseBookList;
    QHash<QString, PhraseBook *> phraseBookHash;
    foreach (PhraseBook *pb, m_phraseBooks) {
        if (pb->language() != QLocale::C && m_dataModel->language(m_currentIndex.model()) != QLocale::C) {
            if (pb->language() != m_dataModel->language(m_currentIndex.model()))
                continue;
            if (pb->country() == m_dataModel->model(m_currentIndex.model())->country())
                phraseBookList.prepend(pb->friendlyPhraseBookName());
            else
                phraseBookList.append(pb->friendlyPhraseBookName());
        } else {
            phraseBookList.append(pb->friendlyPhraseBookName());
        }
        phraseBookHash.insert(pb->friendlyPhraseBookName(), pb);
    }
    if (phraseBookList.isEmpty()) {
        QMessageBox::warning(this, tr("Add to phrase book"),
              tr("No appropriate phrasebook found."));
    } else if (phraseBookList.size() == 1) {
        if (QMessageBox::information(this, tr("Add to phrase book"),
              tr("Adding entry to phrasebook %1").arg(phraseBookList.at(0)),
               QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok)
                              == QMessageBox::Ok)
            phraseBookHash.value(phraseBookList.at(0))->append(phrase);
    } else {
        bool okPressed = false;
        QString selection = QInputDialog::getItem(this, tr("Add to phrase book"),
                                tr("Select phrase book to add to"),
                                phraseBookList, 0, false, &okPressed);
        if (okPressed)
            phraseBookHash.value(selection)->append(phrase);
    }
}

void MainWindow::resetSorting()
{
    m_contextView->sortByColumn(-1, Qt::AscendingOrder);
    m_messageView->sortByColumn(-1, Qt::AscendingOrder);
}

void MainWindow::manual()
{
    if (!m_assistantProcess)
        m_assistantProcess = new QProcess();

    if (m_assistantProcess->state() != QProcess::Running) {
        QString app = QLibraryInfo::location(QLibraryInfo::BinariesPath) + QDir::separator();
#if !defined(Q_OS_MAC)
        app += QLatin1String("assistant");
#else
        app += QLatin1String("Assistant.app/Contents/MacOS/Assistant");
#endif

        m_assistantProcess->start(app, QStringList() << QLatin1String("-enableRemoteControl"));
        if (!m_assistantProcess->waitForStarted()) {
            QMessageBox::critical(this, tr("Qt Linguist"),
                tr("Unable to launch Qt Assistant (%1)").arg(app));
            return;
        }
    }

    QTextStream str(m_assistantProcess);
    str << QLatin1String("SetSource qthelp://com.trolltech.linguist.")
        << (QT_VERSION >> 16) << ((QT_VERSION >> 8) & 0xFF)
        << (QT_VERSION & 0xFF)
        << QLatin1String("/qdoc/linguist-manual.html")
        << QLatin1Char('\0') << endl;
}

void MainWindow::about()
{
    QMessageBox box(this);
    box.setTextFormat(Qt::RichText);
    QString version = tr("Version %1");
#if QT_EDITION == QT_EDITION_OPENSOURCE
    QString open = tr(" Open Source Edition");
    version.append(open);
#endif
    version = version.arg(QLatin1String(QT_VERSION_STR));

    QString edition =
#if QT_EDITION == QT_EDITION_OPENSOURCE
        tr("This version of Qt Linguist is part of the Qt Open Source Edition, for use "
        "in the development of Open Source applications. "
        "Qt is a comprehensive C++ framework for cross-platform application "
        "development.<br/><br/>"
        "You need a commercial Qt license for development of proprietary (closed "
        "source) applications. Please see <tt>http://qtsoftware.com/company/model"
        ".html</tt> for an overview of Qt licensing.");
#elif defined(QT_PRODUCT_LICENSE)
        tr("This program is licensed to you under the terms of the "
        "Qt %1 License Agreement. For details, see the license file "
        "that came with this software distribution.").arg(QLatin1String(QT_PRODUCT_LICENSE));
#else
        tr("This program is licensed to you under the terms of the "
        "Qt Commercial License Agreement. For details, see the file LICENSE "
        "that came with this software distribution.");
#endif

    box.setText(tr("<center><img src=\":/images/splash.png\"/></img><p>%1</p></center>"
                    "<p>Qt Linguist is a tool for adding translations to Qt "
                    "applications.</p>"
                    "<p>%2</p>"
                    "<p>Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies)."
                    "</p><p>The program is provided AS IS with NO WARRANTY OF ANY KIND,"
                    " INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A"
                    " PARTICULAR PURPOSE.</p>").arg(version).arg(edition));

    box.setWindowTitle(QApplication::translate("AboutDialog", "Qt Linguist"));
    box.setIcon(QMessageBox::NoIcon);
    box.exec();
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this, tr("Qt Linguist"));
}

void MainWindow::setupPhrase()
{
    bool enabled = !m_phraseBooks.isEmpty();
    m_ui.menuClosePhraseBook->setEnabled(enabled);
    m_ui.menuEditPhraseBook->setEnabled(enabled);
    m_ui.menuPrintPhraseBook->setEnabled(enabled);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (maybeSaveAll() && closePhraseBooks())
        e->accept();
    else
        e->ignore();
}

bool MainWindow::maybeSaveAll()
{
    if (!m_dataModel->isModified())
        return true;

    switch (QMessageBox::information(this, tr("Qt Linguist"),
        tr("Do you want to save the modified files?"),
        QMessageBox::Yes | QMessageBox::Default,
        QMessageBox::No,
        QMessageBox::Cancel | QMessageBox::Escape))
    {
        case QMessageBox::Cancel:
            return false;
        case QMessageBox::Yes:
            saveAll();
            return !m_dataModel->isModified();
        case QMessageBox::No:
            break;
    }
    return true;
}

bool MainWindow::maybeSave(int model)
{
    if (!m_dataModel->isModified(model))
        return true;

    switch (QMessageBox::information(this, tr("Qt Linguist"),
        tr("Do you want to save '%1'?").arg(m_dataModel->srcFileName(model, true)),
        QMessageBox::Yes | QMessageBox::Default,
        QMessageBox::No,
        QMessageBox::Cancel | QMessageBox::Escape))
    {
        case QMessageBox::Cancel:
            return false;
        case QMessageBox::Yes:
            saveInternal(model);
            return !m_dataModel->isModified(model);
        case QMessageBox::No:
            break;
    }
    return true;
}

void MainWindow::updateCaption()
{
    QString cap;
    bool enable = false;
    bool enableRw = false;
    for (int i = 0; i < m_dataModel->modelCount(); ++i) {
        enable = true;
        if (m_dataModel->isModelWritable(i)) {
            enableRw = true;
            break;
        }
    }
    m_ui.actionSaveAll->setEnabled(enableRw);
    m_ui.actionReleaseAll->setEnabled(enableRw);
    m_ui.actionCloseAll->setEnabled(enable);
    m_ui.actionPrint->setEnabled(enable);
    m_ui.actionAccelerators->setEnabled(enable);
    m_ui.actionEndingPunctuation->setEnabled(enable);
    m_ui.actionPhraseMatches->setEnabled(enable);
    m_ui.actionPlaceMarkerMatches->setEnabled(enable);
    m_ui.actionResetSorting->setEnabled(enable);

    updateActiveModel(m_messageEditor->activeModel());
    // Ensure that the action labels get updated
    m_fileActiveModel = m_editActiveModel = -2;

    if (!enable)
        cap = tr("Qt Linguist[*]");
    else
        cap = tr("%1[*] - Qt Linguist").arg(m_dataModel->condensedSrcFileNames(true));
    setWindowTitle(cap);
}

void MainWindow::selectedContextChanged(const QModelIndex &sortedIndex, const QModelIndex &oldIndex)
{
    if (sortedIndex.isValid()) {
        if (m_settingCurrentMessage)
            return; // Avoid playing ping-pong with the current message

        QModelIndex sourceIndex = m_sortedContextsModel->mapToSource(sortedIndex);
        if (m_messageModel->parent(currentMessageIndex()).row() == sourceIndex.row())
            return;

        QModelIndex contextIndex = setMessageViewRoot(sourceIndex);
        const QModelIndex &firstChild =
                m_sortedMessagesModel->index(0, sourceIndex.column(), contextIndex);
        m_messageView->setCurrentIndex(firstChild);
    } else if (oldIndex.isValid()) {
        m_contextView->setCurrentIndex(oldIndex);
    }
}

/*
 * Updates the message displayed in the message editor and related actions.
 */
void MainWindow::selectedMessageChanged(const QModelIndex &sortedIndex, const QModelIndex &oldIndex)
{
    // Keep a valid selection whenever possible
    if (!sortedIndex.isValid() && oldIndex.isValid()) {
        m_messageView->setCurrentIndex(oldIndex);
        return;
    }

    QModelIndex index = m_sortedMessagesModel->mapToSource(sortedIndex);
    if (index.isValid()) {
        int model = (index.column() && (index.column() - 1 < m_dataModel->modelCount())) ?
                        index.column() - 1 : m_currentIndex.model();
        m_currentIndex = m_messageModel->dataIndex(index, model);
        m_messageEditor->showMessage(m_currentIndex);
        MessageItem *m = 0;
        if (model >= 0 && (m = m_dataModel->messageItem(m_currentIndex))) {
            if (m_dataModel->isModelWritable(model) && !m->isObsolete())
                m_phraseView->setSourceText(m_currentIndex.model(), m->text());
            else
                m_phraseView->setSourceText(-1, QString());
        } else {
            if (model < 0) {
                model = m_dataModel->multiContextItem(m_currentIndex.context())
                                ->firstNonobsoleteMessageIndex(m_currentIndex.message());
                if (model >= 0)
                    m = m_dataModel->messageItem(m_currentIndex, model);
            }
            m_phraseView->setSourceText(-1, QString());
        }
        if (m) {
            if (hasFormPreview(m->fileName())) {
                m_sourceAndFormView->setCurrentWidget(m_formPreviewView);
                m_formPreviewView->setSourceContext(model, m);
            } else {
                m_sourceAndFormView->setCurrentWidget(m_sourceCodeView);
                QDir dir = QFileInfo(m_dataModel->srcFileName(model)).dir();
                QString fileName = dir.absoluteFilePath(m->fileName());
                m_sourceCodeView->setSourceContext(fileName, m->lineNumber());
            }
            m_errorsView->setEnabled(true);
        } else {
            m_sourceAndFormView->setCurrentWidget(m_sourceCodeView);
            m_sourceCodeView->setSourceContext(QString(), 0);
            m_errorsView->setEnabled(false);
        }
        updateDanger(m_currentIndex, true);
    } else {
        m_currentIndex = MultiDataIndex();
        m_messageEditor->showNothing();
        m_phraseView->setSourceText(-1, QString());
        m_sourceAndFormView->setCurrentWidget(m_sourceCodeView);
        m_sourceCodeView->setSourceContext(QString(), 0);
    }

    updatePhraseBookActions();
    m_ui.actionSelectAll->setEnabled(index.isValid());
}

void MainWindow::translationChanged(const MultiDataIndex &index)
{
    // We get that as a result of batch translation or search & translate,
    // so the current model is known to match.
    if (index != m_currentIndex)
        return;

    m_messageEditor->showMessage(index);
    updateDanger(index, true);

    MessageItem *m = m_dataModel->messageItem(index);
    if (hasFormPreview(m->fileName()))
        m_formPreviewView->setSourceContext(index.model(), m);
}

// This and the following function operate directly on the messageitem,
// so the model does not emit modification notifications.
void MainWindow::updateTranslation(const QStringList &translations)
{
    MessageItem *m = m_dataModel->messageItem(m_currentIndex);
    if (!m)
        return;
    if (translations == m->translations())
        return;

    m->setTranslations(translations);
    if (hasFormPreview(m->fileName()))
        m_formPreviewView->setSourceContext(m_currentIndex.model(), m);
    updateDanger(m_currentIndex, true);

    if (m->isFinished())
        m_dataModel->setFinished(m_currentIndex, false);
    else
        m_dataModel->setModified(m_currentIndex.model(), true);
}

void MainWindow::updateTranslatorComment(const QString &comment)
{
    MessageItem *m = m_dataModel->messageItem(m_currentIndex);
    if (!m)
        return;
    if (comment == m->translatorComment())
        return;

    m->setTranslatorComment(comment);

    m_dataModel->setModified(m_currentIndex.model(), true);
}

void MainWindow::refreshItemViews()
{
    m_messageModel->blockSignals(false);
    m_contextView->update();
    m_messageView->update();
    setWindowModified(m_dataModel->isModified());
    m_modifiedLabel->setVisible(m_dataModel->isModified());
    updateStatistics();
}

void MainWindow::doneAndNext()
{
    int model = m_messageEditor->activeModel();
    if (model >= 0 && m_dataModel->isModelWritable(model))
        m_dataModel->setFinished(m_currentIndex, true);

    if (!m_messageEditor->focusNextUnfinished())
        nextUnfinished();
}

void MainWindow::toggleFinished(const QModelIndex &index)
{
    if (!index.isValid() || index.column() - 1 >= m_dataModel->modelCount()
        || !m_dataModel->isModelWritable(index.column() - 1) || index.parent() == QModelIndex())
        return;

    QModelIndex item = m_sortedMessagesModel->mapToSource(index);
    MultiDataIndex dataIndex = m_messageModel->dataIndex(item);
    MessageItem *m = m_dataModel->messageItem(dataIndex);

    if (!m || m->message().type() == TranslatorMessage::Obsolete)
        return;

    m_dataModel->setFinished(dataIndex, !m->isFinished());
}

/*
 * Receives a context index in the sorted messages model and returns the next
 * logical context index in the same model, based on the sort order of the
 * contexts in the sorted contexts model.
 */
QModelIndex MainWindow::nextContext(const QModelIndex &index) const
{
    QModelIndex sortedContextIndex = m_sortedContextsModel->mapFromSource(
            m_sortedMessagesModel->mapToSource(index));

    int nextRow = sortedContextIndex.row() + 1;
    if (nextRow >= m_sortedContextsModel->rowCount())
        nextRow = 0;
    sortedContextIndex = m_sortedContextsModel->index(nextRow, index.column());

    return m_sortedMessagesModel->mapFromSource(
            m_sortedContextsModel->mapToSource(sortedContextIndex));
}

/*
 * See nextContext.
 */
QModelIndex MainWindow::prevContext(const QModelIndex &index) const
{
    QModelIndex sortedContextIndex = m_sortedContextsModel->mapFromSource(
            m_sortedMessagesModel->mapToSource(index));

    int prevRow = sortedContextIndex.row() - 1;
    if (prevRow < 0) prevRow = m_sortedContextsModel->rowCount() - 1;
    sortedContextIndex = m_sortedContextsModel->index(prevRow, index.column());

    return m_sortedMessagesModel->mapFromSource(
            m_sortedContextsModel->mapToSource(sortedContextIndex));
}

QModelIndex MainWindow::nextMessage(const QModelIndex &currentIndex, bool checkUnfinished) const
{
    QModelIndex idx = currentIndex.isValid() ? currentIndex : m_sortedMessagesModel->index(0, 0);
    do {
        int row = 0;
        QModelIndex par = idx.parent();
        if (par.isValid()) {
            row = idx.row() + 1;
        } else {        // In case we are located on a top-level node
            par = idx;
        }

        if (row >= m_sortedMessagesModel->rowCount(par)) {
            par = nextContext(par);
            row = 0;
        }
        idx = m_sortedMessagesModel->index(row, idx.column(), par);

        if (!checkUnfinished)
            return idx;

        QModelIndex item = m_sortedMessagesModel->mapToSource(idx);
        MultiDataIndex index = m_messageModel->dataIndex(item, -1);
        if (m_dataModel->multiMessageItem(index)->isUnfinished())
            return idx;
    } while (idx != currentIndex);
    return QModelIndex();
}

QModelIndex MainWindow::prevMessage(const QModelIndex &currentIndex, bool checkUnfinished) const
{
    QModelIndex idx = currentIndex.isValid() ? currentIndex : m_sortedMessagesModel->index(0, 0);
    do {
        int row = idx.row() - 1;
        QModelIndex par = idx.parent();
        if (!par.isValid()) {   // In case we are located on a top-level node
            par = idx;
            row = -1;
        }

        if (row < 0) {
            par = prevContext(par);
            row = m_sortedMessagesModel->rowCount(par) - 1;
        }
        idx = m_sortedMessagesModel->index(row, idx.column(), par);

        if (!checkUnfinished)
            return idx;

        QModelIndex item = m_sortedMessagesModel->mapToSource(idx);
        MultiDataIndex index = m_messageModel->dataIndex(item, -1);
        if (m_dataModel->multiMessageItem(index)->isUnfinished())
            return idx;
    } while (idx != currentIndex);
    return QModelIndex();
}

void MainWindow::nextUnfinished()
{
    if (m_ui.actionNextUnfinished->isEnabled()) {
        if (!next(true)) {
            // If no Unfinished message is left, the user has finished the job.  We
            // congratulate on a job well done with this ringing bell.
            statusBar()->showMessage(tr("No untranslated translation units left."), MessageMS);
            qApp->beep();
        }
    }
}

void MainWindow::prevUnfinished()
{
    if (m_ui.actionNextUnfinished->isEnabled()) {
        if (!prev(true)) {
            // If no Unfinished message is left, the user has finished the job.  We
            // congratulate on a job well done with this ringing bell.
            statusBar()->showMessage(tr("No untranslated translation units left."), MessageMS);
            qApp->beep();
        }
    }
}

void MainWindow::prev()
{
    prev(false);
}

void MainWindow::next()
{
    next(false);
}

bool MainWindow::prev(bool checkUnfinished)
{
    QModelIndex index = prevMessage(m_messageView->currentIndex(), checkUnfinished);
    if (index.isValid())
        setCurrentMessage(m_sortedMessagesModel->mapToSource(index));
    if (checkUnfinished)
        m_messageEditor->setUnfinishedEditorFocus();
    else
        m_messageEditor->setEditorFocus();
    return index.isValid();
}

bool MainWindow::next(bool checkUnfinished)
{
    QModelIndex index = nextMessage(m_messageView->currentIndex(), checkUnfinished);
    if (index.isValid())
        setCurrentMessage(m_sortedMessagesModel->mapToSource(index));
    if (checkUnfinished)
        m_messageEditor->setUnfinishedEditorFocus();
    else
        m_messageEditor->setEditorFocus();
    return index.isValid();
}

void MainWindow::findNext(const QString &text, DataModel::FindLocation where, bool matchCase, bool ignoreAccelerators)
{
    if (text.isEmpty())
        return;
    m_findText = text;
    m_findWhere = where;
    m_findMatchCase = matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive;
    m_findIgnoreAccelerators = ignoreAccelerators;
    m_ui.actionFindNext->setEnabled(true);
    findAgain();
}

void MainWindow::revalidate()
{
    for (MultiDataModelIterator it(m_dataModel, -1); it.isValid(); ++it)
        updateDanger(it, false);

    if (m_currentIndex.isValid())
        updateDanger(m_currentIndex, true);
}

QString MainWindow::friendlyString(const QString& str)
{
    QString f = str.toLower();
    f.replace(QRegExp(QString(QLatin1String("[.,:;!?()-]"))), QString(QLatin1String(" ")));
    f.remove(QLatin1Char('&'));
    return f.simplified();
}

void MainWindow::setupMenuBar()
{
    m_ui.actionAccelerators->setIcon(QIcon(resourcePrefix() + QLatin1String("/accelerator.png")));
    m_ui.actionOpenPhraseBook->setIcon(QIcon(resourcePrefix() + QLatin1String("/book.png")));
    m_ui.actionDoneAndNext->setIcon(QIcon(resourcePrefix() + QLatin1String("/doneandnext.png")));
    m_ui.actionCopy->setIcon(QIcon(resourcePrefix() + QLatin1String("/editcopy.png")));
    m_ui.actionCut->setIcon(QIcon(resourcePrefix() + QLatin1String("/editcut.png")));
    m_ui.actionPaste->setIcon(QIcon(resourcePrefix() + QLatin1String("/editpaste.png")));
    m_ui.actionOpen->setIcon(QIcon(resourcePrefix() + QLatin1String("/fileopen.png")));
    m_ui.actionOpenAux->setIcon(QIcon(resourcePrefix() + QLatin1String("/fileopen.png")));
    m_ui.actionSaveAll->setIcon(QIcon(resourcePrefix() + QLatin1String("/filesave.png")));
    m_ui.actionSave->setIcon(QIcon(resourcePrefix() + QLatin1String("/filesave.png")));
    m_ui.actionNext->setIcon(QIcon(resourcePrefix() + QLatin1String("/next.png")));
    m_ui.actionNextUnfinished->setIcon(QIcon(resourcePrefix() + QLatin1String("/nextunfinished.png")));
    m_ui.actionPhraseMatches->setIcon(QIcon(resourcePrefix() + QLatin1String("/phrase.png")));
    m_ui.actionEndingPunctuation->setIcon(QIcon(resourcePrefix() + QLatin1String("/punctuation.png")));
    m_ui.actionPrev->setIcon(QIcon(resourcePrefix() + QLatin1String("/prev.png")));
    m_ui.actionPrevUnfinished->setIcon(QIcon(resourcePrefix() + QLatin1String("/prevunfinished.png")));
    m_ui.actionPrint->setIcon(QIcon(resourcePrefix() + QLatin1String("/print.png")));
    m_ui.actionRedo->setIcon(QIcon(resourcePrefix() + QLatin1String("/redo.png")));
    m_ui.actionFind->setIcon(QIcon(resourcePrefix() + QLatin1String("/searchfind.png")));
    m_ui.actionUndo->setIcon(QIcon(resourcePrefix() + QLatin1String("/undo.png")));
    m_ui.actionPlaceMarkerMatches->setIcon(QIcon(resourcePrefix() + QLatin1String("/validateplacemarkers.png")));
    m_ui.actionWhatsThis->setIcon(QIcon(resourcePrefix() + QLatin1String("/whatsthis.png")));

    // File menu
    connect(m_ui.menuFile, SIGNAL(aboutToShow()), SLOT(fileAboutToShow()));
    connect(m_ui.actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(m_ui.actionOpenAux, SIGNAL(triggered()), this, SLOT(openAux()));
    connect(m_ui.actionSaveAll, SIGNAL(triggered()), this, SLOT(saveAll()));
    connect(m_ui.actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(m_ui.actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(m_ui.actionReleaseAll, SIGNAL(triggered()), this, SLOT(releaseAll()));
    connect(m_ui.actionRelease, SIGNAL(triggered()), this, SLOT(release()));
    connect(m_ui.actionReleaseAs, SIGNAL(triggered()), this, SLOT(releaseAs()));
    connect(m_ui.actionPrint, SIGNAL(triggered()), this, SLOT(print()));
    connect(m_ui.actionClose, SIGNAL(triggered()), this, SLOT(closeFile()));
    connect(m_ui.actionCloseAll, SIGNAL(triggered()), this, SLOT(closeAll()));
    connect(m_ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));

    // Edit menu
    connect(m_ui.menuEdit, SIGNAL(aboutToShow()), SLOT(editAboutToShow()));

    connect(m_ui.actionUndo, SIGNAL(triggered()), m_messageEditor, SLOT(undo()));
    connect(m_messageEditor, SIGNAL(undoAvailable(bool)), m_ui.actionUndo, SLOT(setEnabled(bool)));

    connect(m_ui.actionRedo, SIGNAL(triggered()), m_messageEditor, SLOT(redo()));
    connect(m_messageEditor, SIGNAL(redoAvailable(bool)), m_ui.actionRedo, SLOT(setEnabled(bool)));

    connect(m_ui.actionCopy, SIGNAL(triggered()), m_messageEditor, SLOT(copy()));
    connect(m_messageEditor, SIGNAL(copyAvailable(bool)), m_ui.actionCopy, SLOT(setEnabled(bool)));

    connect(m_messageEditor, SIGNAL(cutAvailable(bool)), m_ui.actionCut, SLOT(setEnabled(bool)));
    connect(m_ui.actionCut, SIGNAL(triggered()), m_messageEditor, SLOT(cut()));

    connect(m_messageEditor, SIGNAL(pasteAvailable(bool)), m_ui.actionPaste, SLOT(setEnabled(bool)));
    connect(m_ui.actionPaste, SIGNAL(triggered()), m_messageEditor, SLOT(paste()));

    connect(m_ui.actionSelectAll, SIGNAL(triggered()), m_messageEditor, SLOT(selectAll()));
    connect(m_ui.actionFind, SIGNAL(triggered()), m_findDialog, SLOT(find()));
    connect(m_ui.actionFindNext, SIGNAL(triggered()), this, SLOT(findAgain()));
    connect(m_ui.actionSearchAndTranslate, SIGNAL(triggered()), this, SLOT(showTranslateDialog()));
    connect(m_ui.actionBatchTranslation, SIGNAL(triggered()), this, SLOT(showBatchTranslateDialog()));
    connect(m_ui.actionTranslationFileSettings, SIGNAL(triggered()), this, SLOT(showTranslationSettings()));

    connect(m_batchTranslateDialog, SIGNAL(finished()), SLOT(refreshItemViews()));

    // Translation menu
    // when updating the accelerators, remember the status bar
    connect(m_ui.actionPrevUnfinished, SIGNAL(triggered()), this, SLOT(prevUnfinished()));
    connect(m_ui.actionNextUnfinished, SIGNAL(triggered()), this, SLOT(nextUnfinished()));
    connect(m_ui.actionNext, SIGNAL(triggered()), this, SLOT(next()));
    connect(m_ui.actionPrev, SIGNAL(triggered()), this, SLOT(prev()));
    connect(m_ui.actionDoneAndNext, SIGNAL(triggered()), this, SLOT(doneAndNext()));
    connect(m_ui.actionBeginFromSource, SIGNAL(triggered()), m_messageEditor, SLOT(beginFromSource()));
    connect(m_messageEditor, SIGNAL(beginFromSourceAvailable(bool)), m_ui.actionBeginFromSource, SLOT(setEnabled(bool)));

    // Phrasebook menu
    connect(m_ui.actionNewPhraseBook, SIGNAL(triggered()), this, SLOT(newPhraseBook()));
    connect(m_ui.actionOpenPhraseBook, SIGNAL(triggered()), this, SLOT(openPhraseBook()));
    connect(m_ui.menuClosePhraseBook, SIGNAL(triggered(QAction*)),
        this, SLOT(closePhraseBook(QAction*)));
    connect(m_ui.menuEditPhraseBook, SIGNAL(triggered(QAction*)),
        this, SLOT(editPhraseBook(QAction*)));
    connect(m_ui.menuPrintPhraseBook, SIGNAL(triggered(QAction*)),
        this, SLOT(printPhraseBook(QAction*)));
    connect(m_ui.actionAddToPhraseBook, SIGNAL(triggered()), this, SLOT(addToPhraseBook()));

    // Validation menu
    connect(m_ui.actionAccelerators, SIGNAL(triggered()), this, SLOT(revalidate()));
    connect(m_ui.actionEndingPunctuation, SIGNAL(triggered()), this, SLOT(revalidate()));
    connect(m_ui.actionPhraseMatches, SIGNAL(triggered()), this, SLOT(revalidate()));
    connect(m_ui.actionPlaceMarkerMatches, SIGNAL(triggered()), this, SLOT(revalidate()));

    // View menu
    connect(m_ui.actionResetSorting, SIGNAL(triggered()), this, SLOT(resetSorting()));
    connect(m_ui.actionDisplayGuesses, SIGNAL(triggered()), m_phraseView, SLOT(toggleGuessing()));
    connect(m_ui.actionStatistics, SIGNAL(triggered()), this, SLOT(toggleStatistics()));
    connect(m_ui.menuView, SIGNAL(aboutToShow()), this, SLOT(updateViewMenu()));
    m_ui.menuViewViews->addAction(m_contextDock->toggleViewAction());
    m_ui.menuViewViews->addAction(m_messagesDock->toggleViewAction());
    m_ui.menuViewViews->addAction(m_phrasesDock->toggleViewAction());
    m_ui.menuViewViews->addAction(m_sourceAndFormDock->toggleViewAction());
    m_ui.menuViewViews->addAction(m_errorsDock->toggleViewAction());

#if defined(Q_WS_MAC)
    // Window menu
    QMenu *windowMenu = new QMenu(tr("&Window"), this);
    menuBar()->insertMenu(m_ui.menuHelp->menuAction(), windowMenu);
    windowMenu->addAction(tr("Minimize"), this,
        SLOT(showMinimized()), QKeySequence(tr("Ctrl+M")));
#endif

    // Help
    connect(m_ui.actionManual, SIGNAL(triggered()), this, SLOT(manual()));
    connect(m_ui.actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(m_ui.actionAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
    connect(m_ui.actionWhatsThis, SIGNAL(triggered()), this, SLOT(onWhatsThis()));

    connect(m_ui.menuRecentlyOpenedFiles, SIGNAL(triggered(QAction*)), this,
        SLOT(recentFileActivated(QAction*)));

    m_ui.actionManual->setWhatsThis(tr("Display the manual for %1.").arg(tr("Qt Linguist")));
    m_ui.actionAbout->setWhatsThis(tr("Display information about %1.").arg(tr("Qt Linguist")));
    m_ui.actionDoneAndNext->setShortcuts(QList<QKeySequence>()
                                            << QKeySequence(QLatin1String("Ctrl+Return"))
                                            << QKeySequence(QLatin1String("Ctrl+Enter")));

    // Disable the Close/Edit/Print phrasebook menuitems if they are not loaded
    connect(m_ui.menuPhrases, SIGNAL(aboutToShow()), this, SLOT(setupPhrase()));

    connect(m_ui.menuRecentlyOpenedFiles, SIGNAL(aboutToShow()), SLOT(setupRecentFilesMenu()));
}

void MainWindow::updateActiveModel(int model)
{
    if (model >= 0)
        updateLatestModel(model);
}

// Arriving here implies that the messageEditor does not have focus
void MainWindow::updateLatestModel(const QModelIndex &index)
{
    if (index.column() && (index.column() - 1 < m_dataModel->modelCount()))
        updateLatestModel(index.column() - 1);
}

void MainWindow::updateLatestModel(int model)
{
    m_currentIndex = MultiDataIndex(model, m_currentIndex.context(), m_currentIndex.message());
    bool enable = false;
    bool enableRw = false;
    if (model >= 0) {
        enable = true;
        if (m_dataModel->isModelWritable(model))
            enableRw = true;

        if (m_currentIndex.isValid()) {
            if (MessageItem *item = m_dataModel->messageItem(m_currentIndex)) {
                if (hasFormPreview(item->fileName()))
                    m_formPreviewView->setSourceContext(model, item);
                if (enableRw && !item->isObsolete())
                    m_phraseView->setSourceText(model, item->text());
                else
                    m_phraseView->setSourceText(-1, QString());
            } else {
                m_phraseView->setSourceText(-1, QString());
            }
        }
    }
    m_ui.actionSave->setEnabled(enableRw);
    m_ui.actionSaveAs->setEnabled(enableRw);
    m_ui.actionRelease->setEnabled(enableRw);
    m_ui.actionReleaseAs->setEnabled(enableRw);
    m_ui.actionClose->setEnabled(enable);
    m_ui.actionTranslationFileSettings->setEnabled(enableRw);
    m_ui.actionSearchAndTranslate->setEnabled(enableRw);
    // cut & paste - edit only
    updatePhraseBookActions();
    updateStatistics();
}

// Note for *AboutToShow: Due to the delayed nature, only actions without shortcuts
// and representations outside the menu may be setEnabled()/setVisible() here.

void MainWindow::fileAboutToShow()
{
    if (m_fileActiveModel != m_currentIndex.model()) {
        // We rename the actions so the shortcuts need not be reassigned.
        bool en;
        if (m_dataModel->modelCount() > 1) {
            if (m_currentIndex.model() >= 0) {
                QString fn = QFileInfo(m_dataModel->srcFileName(m_currentIndex.model())).baseName();
                m_ui.actionSave->setText(tr("&Save '%1'").arg(fn));
                m_ui.actionSaveAs->setText(tr("Save '%1' &As...").arg(fn));
                m_ui.actionRelease->setText(tr("Release '%1'").arg(fn));
                m_ui.actionReleaseAs->setText(tr("Release '%1' As...").arg(fn));
                m_ui.actionClose->setText(tr("&Close '%1'").arg(fn));
            } else {
                m_ui.actionSave->setText(tr("&Save"));
                m_ui.actionSaveAs->setText(tr("Save &As..."));
                m_ui.actionRelease->setText(tr("Release"));
                m_ui.actionReleaseAs->setText(tr("Release As..."));
                m_ui.actionClose->setText(tr("&Close"));
            }

            m_ui.actionSaveAll->setText(tr("Save All"));
            m_ui.actionReleaseAll->setText(tr("&Release All"));
            m_ui.actionCloseAll->setText(tr("Close All"));
            en = true;
        } else {
            m_ui.actionSaveAs->setText(tr("Save &As..."));
            m_ui.actionReleaseAs->setText(tr("Release As..."));

            m_ui.actionSaveAll->setText(tr("&Save"));
            m_ui.actionReleaseAll->setText(tr("&Release"));
            m_ui.actionCloseAll->setText(tr("&Close"));
            en = false;
        }
        m_ui.actionSave->setVisible(en);
        m_ui.actionRelease->setVisible(en);
        m_ui.actionClose->setVisible(en);
        m_fileActiveModel = m_currentIndex.model();
    }
}

void MainWindow::editAboutToShow()
{
    if (m_editActiveModel != m_currentIndex.model()) {
        if (m_currentIndex.model() >= 0 && m_dataModel->modelCount() > 1) {
            QString fn = QFileInfo(m_dataModel->srcFileName(m_currentIndex.model())).baseName();
            m_ui.actionTranslationFileSettings->setText(tr("Translation File &Settings for '%1'...").arg(fn));
            m_ui.actionBatchTranslation->setText(tr("&Batch Translation of '%1'...").arg(fn));
            m_ui.actionSearchAndTranslate->setText(tr("Search And &Translate in '%1'...").arg(fn));
        } else {
            m_ui.actionTranslationFileSettings->setText(tr("Translation File &Settings..."));
            m_ui.actionBatchTranslation->setText(tr("&Batch Translation..."));
            m_ui.actionSearchAndTranslate->setText(tr("Search And &Translate..."));
        }
        m_editActiveModel = m_currentIndex.model();
    }
}

void MainWindow::updateViewMenu()
{
    bool check = m_statistics ? m_statistics->isVisible() : false;
    m_ui.actionStatistics->setChecked(check);
}

void MainWindow::showContextDock()
{
    m_contextDock->show();
    m_contextDock->raise();
}

void MainWindow::showMessagesDock()
{
    m_messagesDock->show();
    m_messagesDock->raise();
}

void MainWindow::showPhrasesDock()
{
    m_phrasesDock->show();
    m_phrasesDock->raise();
}

void MainWindow::showSourceCodeDock()
{
    m_sourceAndFormDock->show();
    m_sourceAndFormDock->raise();
}

void MainWindow::showErrorDock()
{
    m_errorsDock->show();
    m_errorsDock->raise();
}

void MainWindow::onWhatsThis()
{
    QWhatsThis::enterWhatsThisMode();
}

void MainWindow::setupToolBars()
{
    QToolBar *filet = new QToolBar(this);
    filet->setObjectName(QLatin1String("FileToolbar"));
    filet->setWindowTitle(tr("File"));
    this->addToolBar(filet);
    m_ui.menuToolbars->addAction(filet->toggleViewAction());

    QToolBar *editt = new QToolBar(this);
    editt->setVisible(false);
    editt->setObjectName(QLatin1String("EditToolbar"));
    editt->setWindowTitle(tr("Edit"));
    this->addToolBar(editt);
    m_ui.menuToolbars->addAction(editt->toggleViewAction());

    QToolBar *translationst = new QToolBar(this);
    translationst->setObjectName(QLatin1String("TranslationToolbar"));
    translationst->setWindowTitle(tr("Translation"));
    this->addToolBar(translationst);
    m_ui.menuToolbars->addAction(translationst->toggleViewAction());

    QToolBar *validationt = new QToolBar(this);
    validationt->setObjectName(QLatin1String("ValidationToolbar"));
    validationt->setWindowTitle(tr("Validation"));
    this->addToolBar(validationt);
    m_ui.menuToolbars->addAction(validationt->toggleViewAction());

    QToolBar *helpt = new QToolBar(this);
    helpt->setVisible(false);
    helpt->setObjectName(QLatin1String("HelpToolbar"));
    helpt->setWindowTitle(tr("Help"));
    this->addToolBar(helpt);
    m_ui.menuToolbars->addAction(helpt->toggleViewAction());


    filet->addAction(m_ui.actionOpen);
    filet->addAction(m_ui.actionSaveAll);
    filet->addAction(m_ui.actionPrint);
    filet->addSeparator();
    filet->addAction(m_ui.actionOpenPhraseBook);

    editt->addAction(m_ui.actionUndo);
    editt->addAction(m_ui.actionRedo);
    editt->addSeparator();
    editt->addAction(m_ui.actionCut);
    editt->addAction(m_ui.actionCopy);
    editt->addAction(m_ui.actionPaste);
    editt->addSeparator();
    editt->addAction(m_ui.actionFind);

    translationst->addAction(m_ui.actionPrev);
    translationst->addAction(m_ui.actionNext);
    translationst->addAction(m_ui.actionPrevUnfinished);
    translationst->addAction(m_ui.actionNextUnfinished);
    translationst->addAction(m_ui.actionDoneAndNext);

    validationt->addAction(m_ui.actionAccelerators);
    validationt->addAction(m_ui.actionEndingPunctuation);
    validationt->addAction(m_ui.actionPhraseMatches);
    validationt->addAction(m_ui.actionPlaceMarkerMatches);

    helpt->addAction(m_ui.actionWhatsThis);
}

QModelIndex MainWindow::setMessageViewRoot(const QModelIndex &index)
{
    const QModelIndex &sortedContextIndex = m_sortedMessagesModel->mapFromSource(index);
    const QModelIndex &trueContextIndex = m_sortedMessagesModel->index(sortedContextIndex.row(), 0);
    if (m_messageView->rootIndex() != trueContextIndex)
        m_messageView->setRootIndex(trueContextIndex);
    return trueContextIndex;
}

/*
 * Updates the selected entries in the context and message views.
 */
void MainWindow::setCurrentMessage(const QModelIndex &index)
{
    const QModelIndex &contextIndex = m_messageModel->parent(index);
    if (!contextIndex.isValid())
        return;

    const QModelIndex &trueIndex = m_messageModel->index(contextIndex.row(), index.column(), QModelIndex());
    m_settingCurrentMessage = true;
    m_contextView->setCurrentIndex(m_sortedContextsModel->mapFromSource(trueIndex));
    m_settingCurrentMessage = false;

    setMessageViewRoot(contextIndex);
    m_messageView->setCurrentIndex(m_sortedMessagesModel->mapFromSource(index));
}

void MainWindow::setCurrentMessage(const QModelIndex &index, int model)
{
    const QModelIndex &theIndex = m_messageModel->index(index.row(), model + 1, index.parent());
    setCurrentMessage(theIndex);
    m_messageEditor->setEditorFocus(model);
}

QModelIndex MainWindow::currentContextIndex() const
{
    return m_sortedContextsModel->mapToSource(m_contextView->currentIndex());
}

QModelIndex MainWindow::currentMessageIndex() const
{
    return m_sortedMessagesModel->mapToSource(m_messageView->currentIndex());
}

PhraseBook *MainWindow::openPhraseBook(const QString& name)
{
    PhraseBook *pb = new PhraseBook();
    bool langGuessed;
    if (!pb->load(name, &langGuessed)) {
        QMessageBox::warning(this, tr("Qt Linguist"),
            tr("Cannot read from phrase book '%1'.").arg(name));
        delete pb;
        return 0;
    }
    if (langGuessed) {
        if (!m_translationSettingsDialog)
            m_translationSettingsDialog = new TranslationSettingsDialog(this);
        m_translationSettingsDialog->setPhraseBook(pb);
        m_translationSettingsDialog->exec();
    }

    m_phraseBooks.append(pb);

    QAction *a = m_ui.menuClosePhraseBook->addAction(pb->friendlyPhraseBookName());
    m_phraseBookMenu[PhraseCloseMenu].insert(a, pb);
    a->setWhatsThis(tr("Close this phrase book."));

    a = m_ui.menuEditPhraseBook->addAction(pb->friendlyPhraseBookName());
    m_phraseBookMenu[PhraseEditMenu].insert(a, pb);
    a->setWhatsThis(tr("Enables you to add, modify, or delete"
        " entries in this phrase book."));

    a = m_ui.menuPrintPhraseBook->addAction(pb->friendlyPhraseBookName());
    m_phraseBookMenu[PhrasePrintMenu].insert(a, pb);
    a->setWhatsThis(tr("Print the entries in this phrase book."));

    connect(pb, SIGNAL(listChanged()), this, SLOT(updatePhraseDicts()));
    updatePhraseDicts();
    updatePhraseBookActions();

    return pb;
}

bool MainWindow::savePhraseBook(QString *name, PhraseBook &pb)
{
    if (!name->contains(QLatin1Char('.')))
        *name += QLatin1String(".qph");

    if (!pb.save(*name)) {
        QMessageBox::warning(this, tr("Qt Linguist"),
            tr("Cannot create phrase book '%1'.").arg(*name));
        return false;
    }
    return true;
}

bool MainWindow::maybeSavePhraseBook(PhraseBook *pb)
{
    if (pb->isModified())
        switch (QMessageBox::information(this, tr("Qt Linguist"),
            tr("Do you want to save phrase book '%1'?").arg(pb->friendlyPhraseBookName()),
            QMessageBox::Yes | QMessageBox::Default,
            QMessageBox::No,
            QMessageBox::Cancel | QMessageBox::Escape))
        {
            case QMessageBox::Cancel:
                return false;
            case QMessageBox::Yes:
                if (!pb->save(pb->fileName()))
                    return false;
                break;
            case QMessageBox::No:
                break;
        }
    return true;
}

bool MainWindow::closePhraseBooks()
{
    foreach(PhraseBook *phraseBook, m_phraseBooks)
        if (!maybeSavePhraseBook(phraseBook))
            return false;
    return true;
}

void MainWindow::updateProgress()
{
    int numEditable = m_dataModel->getNumEditable();
    int numFinished = m_dataModel->getNumFinished();
    if (!m_dataModel->modelCount())
        m_progressLabel->setText(QString(QLatin1String("    ")));
    else
        m_progressLabel->setText(QString(QLatin1String(" %1/%2 "))
                                 .arg(numFinished).arg(numEditable));
    bool enable = numFinished != numEditable;
    m_ui.actionPrevUnfinished->setEnabled(enable);
    m_ui.actionNextUnfinished->setEnabled(enable);
    m_ui.actionDoneAndNext->setEnabled(enable);

    m_ui.actionPrev->setEnabled(m_dataModel->contextCount() > 0);
    m_ui.actionNext->setEnabled(m_dataModel->contextCount() > 0);
}

void MainWindow::updatePhraseBookActions()
{
    bool phraseBookLoaded = (m_currentIndex.model() >= 0) && !m_phraseBooks.isEmpty();
    m_ui.actionBatchTranslation->setEnabled(m_dataModel->contextCount() > 0 && phraseBookLoaded
                                            && m_dataModel->isModelWritable(m_currentIndex.model()));
    m_ui.actionAddToPhraseBook->setEnabled(currentMessageIndex().isValid() && phraseBookLoaded);
}

void MainWindow::updatePhraseDictInternal(int model)
{
    QHash<QString, QList<Phrase *> > &pd = m_phraseDict[model];

    pd.clear();
    foreach (PhraseBook *pb, m_phraseBooks) {
        bool before;
        if (pb->language() != QLocale::C && m_dataModel->language(model) != QLocale::C) {
            if (pb->language() != m_dataModel->language(model))
                continue;
            before = (pb->country() == m_dataModel->model(model)->country());
        } else {
            before = false;
        }
        foreach (Phrase *p, pb->phrases()) {
            QString f = friendlyString(p->source());
            if (f.length() > 0) {
                f = f.split(QLatin1Char(' ')).first();
                if (!pd.contains(f)) {
                    pd.insert(f, QList<Phrase *>());
                }
                if (before)
                    pd[f].prepend(p);
                else
                    pd[f].append(p);
            }
        }
    }
}

void MainWindow::updatePhraseDict(int model)
{
    updatePhraseDictInternal(model);
    m_phraseView->update();
}

void MainWindow::updatePhraseDicts()
{
    for (int i = 0; i < m_phraseDict.size(); ++i)
        if (!m_dataModel->isModelWritable(i))
            m_phraseDict[i].clear();
        else
            updatePhraseDictInternal(i);
    revalidate();
    m_phraseView->update();
}

void MainWindow::updateDanger(const MultiDataIndex &index, bool verbose)
{
    MultiDataIndex curIdx = index;
    m_errorsView->clear();

    QString source;
    for (int mi = 0; mi < m_dataModel->modelCount(); ++mi) {
        if (!m_dataModel->isModelWritable(mi))
            continue;
        curIdx.setModel(mi);
        MessageItem *m = m_dataModel->messageItem(curIdx);
        if (!m || m->isObsolete())
            continue;

        bool danger = false;
        if (m->message().isTranslated()) {
            if (source.isEmpty()) {
                source = m->pluralText();
                if (source.isEmpty())
                    source = m->text();
            }
            QStringList translations = m->translations();

            if (m_ui.actionAccelerators->isChecked()) {
                bool sk = !QKeySequence::mnemonic(source).isEmpty();
                bool tk = true;
                for (int i = 0; i < translations.count() && tk; ++i) {
                    tk &= !QKeySequence::mnemonic(translations[i]).isEmpty();
                }

                if (!sk && tk) {
                    if (verbose)
                        m_errorsView->addError(mi, ErrorsView::SuperfluousAccelerator);
                    danger = true;
                } else if (sk && !tk) {
                    if (verbose)
                        m_errorsView->addError(mi, ErrorsView::MissingAccelerator);
                    danger = true;
                }
            }
            if (m_ui.actionEndingPunctuation->isChecked()) {
                bool endingok = true;
                for (int i = 0; i < translations.count() && endingok; ++i) {
                    endingok &= (ending(source, m_dataModel->sourceLanguage(mi)) ==
                                ending(translations[i], m_dataModel->language(mi)));
                }

                if (!endingok) {
                    if (verbose)
                        m_errorsView->addError(mi, ErrorsView::PunctuationDiffer);
                    danger = true;
                }
            }
            if (m_ui.actionPhraseMatches->isChecked()) {
                QString fsource = friendlyString(source);
                QString ftranslation = friendlyString(translations.first());
                QStringList lookupWords = fsource.split(QLatin1Char(' '));

                bool phraseFound;
                foreach (const QString &s, lookupWords) {
                    if (m_phraseDict[mi].contains(s)) {
                        phraseFound = true;
                        foreach (const Phrase *p, m_phraseDict[mi].value(s)) {
                            if (fsource == friendlyString(p->source())) {
                                if (ftranslation.indexOf(friendlyString(p->target())) >= 0) {
                                    phraseFound = true;
                                    break;
                                } else {
                                    phraseFound = false;
                                }
                            }
                        }
                        if (!phraseFound) {
                            if (verbose)
                                m_errorsView->addError(mi, ErrorsView::IgnoredPhrasebook, s);
                            danger = true;
                        }
                    }
                }
            }

            if (m_ui.actionPlaceMarkerMatches->isChecked()) {
                // Stores the occurence count of the place markers in the map placeMarkerIndexes.
                // i.e. the occurence count of %1 is stored at placeMarkerIndexes[1],
                // count of %2 is stored at placeMarkerIndexes[2] etc.
                // In the first pass, it counts all place markers in the sourcetext.
                // In the second pass it (de)counts all place markers in the translation.
                // When finished, all elements should have returned to a count of 0,
                // if not there is a mismatch
                // between place markers in the source text and the translation text.
                QHash<int, int> placeMarkerIndexes;
                QString translation;
                int numTranslations = translations.count();
                for (int pass = 0; pass < numTranslations + 1; ++pass) {
                    const QChar *uc_begin = source.unicode();
                    const QChar *uc_end = uc_begin + source.length();
                    if (pass >= 1) {
                        translation = translations[pass - 1];
                        uc_begin = translation.unicode();
                        uc_end = uc_begin + translation.length();
                    }
                    const QChar *c = uc_begin;
                    while (c < uc_end) {
                        if (c->unicode() == '%') {
                            const QChar *escape_start = ++c;
                            while (c->isDigit())
                                ++c;
                            const QChar *escape_end = c;
                            bool ok = true;
                            int markerIndex = QString::fromRawData(
                                    escape_start, escape_end - escape_start).toInt(&ok);
                            if (ok)
                                placeMarkerIndexes[markerIndex] += (pass == 0 ? numTranslations : -1);
                        }
                        ++c;
                    }
                }

                foreach (int i, placeMarkerIndexes) {
                    if (i != 0) {
                        if (verbose)
                            m_errorsView->addError(mi, ErrorsView::PlaceMarkersDiffer);
                        danger = true;
                        break;
                    }
                }

                // Piggy-backed on the general place markers, we check the plural count marker.
                if (m->message().isPlural()) {
                    for (int i = 0; i < numTranslations; ++i)
                        if (m_dataModel->model(mi)->countRefNeeds().at(i)
                            && !translations[i].contains(QLatin1String("%n"))) {
                            if (verbose)
                                m_errorsView->addError(mi, ErrorsView::NumerusMarkerMissing);
                            danger = true;
                            break;
                        }
                }
            }
        }

        if (danger != m->danger())
            m_dataModel->setDanger(curIdx, danger);
    }

    if (verbose)
        statusBar()->showMessage(m_errorsView->firstError());
}

void MainWindow::readConfig()
{
    QString keybase = settingsPrefix();
    QSettings config;

    QRect r(pos(), size());
    restoreGeometry(config.value(keybase + QLatin1String("Geometry/WindowGeometry")).toByteArray());
    restoreState(config.value(keybase + QLatin1String("MainWindowState")).toByteArray());

    m_ui.actionAccelerators->setChecked(
        config.value(keybase + QLatin1String("Validators/Accelerator"), true).toBool());
    m_ui.actionEndingPunctuation->setChecked(
        config.value(keybase + QLatin1String("Validators/EndingPunctuation"), true).toBool());
    m_ui.actionPhraseMatches->setChecked(
        config.value(keybase + QLatin1String("Validators/PhraseMatch"), true).toBool());
    m_ui.actionPlaceMarkerMatches->setChecked(
        config.value(keybase + QLatin1String("Validators/PlaceMarkers"), true).toBool());

    recentFiles().readConfig();

    int size = config.beginReadArray(keybase + QLatin1String("OpenedPhraseBooks"));
    for (int i = 0; i < size; ++i) {
        config.setArrayIndex(i);
        openPhraseBook(config.value(QLatin1String("FileName")).toString());
    }
    config.endArray();
}

void MainWindow::writeConfig()
{
    QString keybase = settingsPrefix();
    QSettings config;
    config.setValue(keybase + QLatin1String("Geometry/WindowGeometry"),
        saveGeometry());
    config.setValue(keybase + QLatin1String("Validators/Accelerator"),
        m_ui.actionAccelerators->isChecked());
    config.setValue(keybase + QLatin1String("Validators/EndingPunctuation"),
        m_ui.actionEndingPunctuation->isChecked());
    config.setValue(keybase + QLatin1String("Validators/PhraseMatch"),
        m_ui.actionPhraseMatches->isChecked());
    config.setValue(keybase + QLatin1String("Validators/PlaceMarkers"),
        m_ui.actionPlaceMarkerMatches->isChecked());
    config.setValue(keybase + QLatin1String("MainWindowState"),
        saveState());
    recentFiles().writeConfig();

    config.beginWriteArray(keybase + QLatin1String("OpenedPhraseBooks"),
        m_phraseBooks.size());
    for (int i = 0; i < m_phraseBooks.size(); ++i) {
        config.setArrayIndex(i);
        config.setValue(QLatin1String("FileName"), m_phraseBooks.at(i)->fileName());
    }
    config.endArray();
}

void MainWindow::setupRecentFilesMenu()
{
    m_ui.menuRecentlyOpenedFiles->clear();
    foreach (const QStringList &strList, recentFiles().filesLists())
        if (strList.size() == 1) {
            const QString &str = strList.first();
            m_ui.menuRecentlyOpenedFiles->addAction(
                    DataModel::prettifyFileName(str))->setData(str);
        } else {
            QMenu *menu = m_ui.menuRecentlyOpenedFiles->addMenu(
                           MultiDataModel::condenseFileNames(
                                MultiDataModel::prettifyFileNames(strList)));
            menu->addAction(tr("All"))->setData(strList);
            foreach (const QString &str, strList)
                menu->addAction(DataModel::prettifyFileName(str))->setData(str);
        }
}

void MainWindow::recentFileActivated(QAction *action)
{
    openFiles(action->data().toStringList());
}

void MainWindow::toggleStatistics()
{
    if (m_ui.actionStatistics->isChecked()) {
        if (!m_statistics) {
            m_statistics = new Statistics(this);
            connect(m_dataModel, SIGNAL(statsChanged(int,int,int,int,int,int)),
                m_statistics, SLOT(updateStats(int,int,int,int,int,int)));
        }
        m_statistics->show();
        updateStatistics();
    }
    else if (m_statistics) {
        m_statistics->close();
    }
}

void MainWindow::maybeUpdateStatistics(const MultiDataIndex &index)
{
    if (index.model() == m_currentIndex.model())
        updateStatistics();
}

void MainWindow::updateStatistics()
{
    // don't call this if stats dialog is not open
    // because this can be slow...
    if (!m_statistics || !m_statistics->isVisible() || m_currentIndex.model() < 0)
        return;

    m_dataModel->model(m_currentIndex.model())->updateStatistics();
}

void MainWindow::showTranslationSettings(int model)
{
    if (!m_translationSettingsDialog)
        m_translationSettingsDialog = new TranslationSettingsDialog(this);
    m_translationSettingsDialog->setDataModel(m_dataModel->model(model));
    m_translationSettingsDialog->exec();
}

void MainWindow::showTranslationSettings()
{
    showTranslationSettings(m_currentIndex.model());
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::DragEnter) {
        QDragEnterEvent *e = static_cast<QDragEnterEvent*>(event);
        if (e->mimeData()->hasFormat(QLatin1String("text/uri-list"))) {
            e->acceptProposedAction();
            return true;
        }
    } else if (event->type() == QEvent::Drop) {
        QDropEvent *e = static_cast<QDropEvent*>(event);
        if (!e->mimeData()->hasFormat(QLatin1String("text/uri-list")))
            return false;
        QStringList urls;
        foreach (QUrl url, e->mimeData()->urls())
            if (!url.toLocalFile().isEmpty())
                urls << url.toLocalFile();
        if (!urls.isEmpty())
            openFiles(urls);
        e->acceptProposedAction();
        return true;
    } else if (event->type() == QEvent::KeyPress) {
        if (static_cast<QKeyEvent *>(event)->key() == Qt::Key_Escape) {
            if (object == m_messageEditor)
                m_messageView->setFocus();
            else if (object == m_messagesDock)
                m_contextView->setFocus();
        }
    }
    return false;
}

QT_END_NAMESPACE
