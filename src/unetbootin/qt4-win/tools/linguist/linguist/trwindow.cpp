/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
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

/*  TRANSLATOR TrWindow

  This is the application's main window.
*/

#include "trwindow.h"
#include "batchtranslationdialog.h"
#include "errorsview.h"
#include "finddialog.h"
#include "languagesdialog.h"
#include "languagesmanager.h"
#include "messagemodel.h"
#include "messageeditor.h"
#include "phrasebookbox.h"
#include "phrasemodel.h"
#include "phraseview.h"
#include "previewtool/trpreviewtool.h"
#include "printout.h"
#include "sortedmessagesmodel.h"
#include "statistics.h"
#include "translatedialog.h"
#include "translationsettingsdialog.h"
#include "translator.h"
#include "sourcecodeview.h"

#include <QAction>
#include <QApplication>
#include <QBitmap>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QProcess>
#include <QTextStream>
#include <QHeaderView>
#include <QLabel>
#include <QLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QMenu>
#include <QRegExp>
#include <QSettings>
#include <QStatusBar>
#include <QToolBar>
#include <QWhatsThis>
#include <QPrintDialog>
#include <QSortFilterProxyModel>
#include <QLibraryInfo>
#include <QUiLoader>
#include <QTableView>
#include <QTranslator>

QT_BEGIN_NAMESPACE

static const int ErrorMS = 600000; // for error messages
static const int MessageMS = 2500;

static const QString SETTINGS_PREFIX = QString::number( (QT_VERSION >> 16) & 0xff )
                    + QLatin1Char('.') + QString::number((QT_VERSION >> 8) & 0xff) + QLatin1Char('/');

RecentFiles TrWindow::m_recentFiles(SETTINGS_PREFIX, 10);
#ifdef Q_WS_MAC
const QString TrWindow::m_resourcePrefix(QLatin1String(":/images/mac"));
#else
const QString TrWindow::m_resourcePrefix(QLatin1String(":/images/win"));
#endif

enum Ending {
    End_None,
    End_FullStop,
    End_Interrobang,
    End_Colon,
    End_Ellipsis
};

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

TrWindow::TrWindow()
    : QMainWindow(0, Qt::Window),
      m_assistantProcess(0),
      findWhere(0),
      findMatchCase(false),
      findIgnoreAccelerators(true),
      foundWhere(0),
      m_translationSettingsDialog(0),
      m_languagesDialog(0),
      m_previewTool(0)
{
    m_ui.setupUi(this);

#ifndef Q_WS_MAC
    setWindowIcon(QPixmap(QLatin1String(":/images/appicon.png") ));
#endif

    MessageModel *mainModel = new MessageModel(this);
    m_languagesManager = new LanguagesManager(mainModel, this);

    // Set up the context dock widget
    m_contextDock = new QDockWidget(this);
    m_contextDock->setObjectName(QLatin1String("ContextDockWidget"));
    m_contextDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_contextDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_contextDock->setWindowTitle(tr("Context"));

    m_sortedContextsModel = new QSortFilterProxyModel(this);
    m_sortedContextsModel->setSortRole(MessageModel::SortRole);
    m_sortedContextsModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortedContextsModel->setSourceModel(mainModel);

    m_contextView = new MessagesTreeView(this);
    m_contextView->setWhatsThis(tr("This panel lists the source contexts."));
    m_contextView->setModel(m_sortedContextsModel);
    m_contextView->header()->resizeSection(0, m_contextView->header()->minimumSectionSize());
    m_contextView->header()->resizeSection(1, 150);

    m_contextDock->setWidget(m_contextView);

    // Set up the messages dock widget
    m_messagesDock = new QDockWidget(this);
    m_messagesDock->setObjectName(QLatin1String("StringsDockWidget"));
    m_messagesDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_messagesDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_messagesDock->setWindowTitle(tr("Strings"));

    m_sortedMessagesModel = new SortedMessagesModel(mainModel, this);

    m_messageView = new QTreeView(m_messagesDock);
    m_messageView->setSortingEnabled(true);
    m_messageView->setRootIsDecorated(false);
    m_messageView->setUniformRowHeights(true);
    m_messageView->setItemsExpandable(false);
    m_messageView->setModel(m_sortedMessagesModel);
    m_messageView->setColumnHidden(3, true);
    m_messageView->header()->setMovable(false);
    m_messageView->header()->resizeSection(0, m_messageView->header()->minimumSectionSize());
    m_messageView->header()->resizeSection(1, 300);

    m_messagesDock->setWidget(m_messageView);

    // Set up main message view
    me = new MessageEditor(m_languagesManager, this);
    // We can't call setCentralWidget(me), since it is already called in m_ui.setupUi()
    QBoxLayout *lout = new QBoxLayout(QBoxLayout::TopToBottom, m_ui.centralwidget);
    lout->addWidget(me);
    lout->setMargin(0);
    m_ui.centralwidget->setLayout(lout);

    // Set up the phrases & guesses dock widget
    m_phrasesDock = new QDockWidget(this);
    m_phrasesDock->setObjectName(QLatin1String("PhrasesDockwidget"));
    m_phrasesDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_phrasesDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_phrasesDock->setWindowTitle(tr("Phrases and guesses"));

    m_phraseView = new PhraseView(mainModel, &m_phraseDict, this);
    m_phrasesDock->setWidget(m_phraseView);

    // Set up source code dock widget
    m_sourceCodeDock = new QDockWidget(this);
    m_sourceCodeDock->setObjectName(QLatin1String("SourceCodeDockWidget"));
    m_sourceCodeDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_sourceCodeDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_sourceCodeDock->setWindowTitle(tr("Source code"));
    m_sourceCodeView = new SourceCodeView(this);
    m_sourceCodeDock->setWidget(m_sourceCodeView);
    connect(m_sourceCodeDock, SIGNAL(visibilityChanged(bool)), m_sourceCodeView, SLOT(setActivated(bool)));

    // Set up errors dock widget
    m_errorsDock = new QDockWidget(this);
    m_errorsDock->setObjectName(QLatin1String("ErrorsDockWidget"));
    m_errorsDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_errorsDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_errorsDock->setWindowTitle(tr("Warnings"));
    m_errorsView = new ErrorsView(this);
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
    addDockWidget(Qt::BottomDockWidgetArea, m_sourceCodeDock);
    addDockWidget(Qt::BottomDockWidgetArea, m_errorsDock);
    tabifyDockWidget(m_errorsDock, m_sourceCodeDock);
    tabifyDockWidget(m_sourceCodeDock, m_phrasesDock);

    // Allow phrases doc to intercept guesses shortcuts
    me->installEventFilter(m_phraseView);

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

    connect(m_phraseView, SIGNAL(phraseSelected(QString)), me, SLOT(setTranslation(QString)));
    connect(m_contextView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(selectedContextChanged(QModelIndex,QModelIndex)));
    connect(m_messageView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(selectedMessageChanged(QModelIndex,QModelIndex)));

    m_translateDialog = new TranslateDialog(this);
    m_batchTranslateDialog = new BatchTranslationDialog(mainModel, this);
    m_findDialog = new FindDialog(this);

    setupMenuBar();
    setupToolBars();

    progress = new QLabel(statusBar());
    statusBar()->addPermanentWidget(progress);
    modified = new QLabel(QString::fromLatin1(" %1 ").arg(tr("MOD")), statusBar());
    statusBar()->addPermanentWidget(modified);

    resetSorting();
    updateProgress();
    updateCaption();

    connect(mainModel, SIGNAL(modelReset()),
            this, SLOT(resetContextView()));
    connect(mainModel, SIGNAL(languageChanged(QLocale::Language)),
            this, SLOT(updateLanguage(QLocale::Language)));
    connect(mainModel, SIGNAL(modifiedChanged(bool)),
            this, SLOT(setWindowModified(bool)));
    connect(mainModel, SIGNAL(modifiedChanged(bool)),
            modified, SLOT(setEnabled(bool)));

    setWindowModified(mainModel->isModified());
    modified->setEnabled(mainModel->isModified());

    connect(m_messageView, SIGNAL(clicked(QModelIndex)), this, SLOT(toggleFinished(QModelIndex)));
    connect(me, SIGNAL(translationChanged(QStringList)), this, SLOT(updateTranslation(QStringList)));
    connect(me, SIGNAL(finished(bool)), this, SLOT(updateFinished(bool)));
    connect(me, SIGNAL(prevUnfinished()), this, SLOT(prevUnfinished()));
    connect(me, SIGNAL(nextUnfinished()), this, SLOT(nextUnfinished()));
    connect(m_findDialog, SIGNAL(findNext(QString,int,bool,bool)),
            this, SLOT(findNext(QString,int,bool,bool)));
    connect(m_translateDialog, SIGNAL(translateAndFindNext(QString,QString,int,int,bool)),
            this, SLOT(translateAndFindNext(QString,QString,int,int,bool)));
    connect(m_languagesManager, SIGNAL(listChanged()), this, SLOT(refreshCurrentMessage()));

    QSize as(qApp->desktop()->size());
    as -= QSize(30, 30);
    resize(QSize(1000, 800).boundedTo(as));
    readConfig();
    stats = 0;
}

void TrWindow::updateLanguage(QLocale::Language lang)
{
    QStringList forms;
    QByteArray rules;

    getNumerusInfo(lang, mainModel()->country(), &rules, &forms);
    me->setNumerusForms(forms);
}

TrWindow::~TrWindow()
{
    writeConfig();
    if (m_assistantProcess && m_assistantProcess->state() == QProcess::Running) {
        m_assistantProcess->terminate();
        m_assistantProcess->waitForFinished(3000);
    }
    qDeleteAll(m_phraseBooks);
    delete m_languagesManager;
    delete stats;
    delete m_previewTool;
}

void TrWindow::openFile(const QString& name)
{
    if (name.isEmpty())
        return;

    statusBar()->showMessage(tr("Loading..."));
    qApp->processEvents();

    if (!mainModel()->load(name)) {
        statusBar()->clearMessage();
        QMessageBox::warning(this, tr("Qt Linguist"), tr("Cannot open '%1'.").arg(name));
        return;
    }

    m_sourceCodeView->setDirectory(QFileInfo(name).absoluteDir());

    foreach (MessageModel *m, m_languagesManager->auxModels())
        m_languagesManager->removeAuxLanguage(m);

    MessageItem *m;
    for (MessageModel::iterator it = mainModel()->begin() ; (m = it.current()) ; ++it)
        updateDanger(m);

    setEnabled(true);
    updateProgress();
    m_filename = name;

    updateCaption();

    m_ui.actionPreviewForm->setEnabled(true);

    statusBar()->showMessage(tr("%n source phrase(s) loaded.", 0, mainModel()->getMessageCount()), MessageMS);
    foundWhere = 0;

    if (mainModel()->contextsInList() > 0) {
        m_ui.actionFind->setEnabled(true);
        m_ui.actionFindNext->setEnabled(false);
        m_ui.actionTranslationFileSettings->setEnabled(true);
        m_ui.actionSearchAndTranslate->setEnabled(true);
        m_ui.actionOtherLanguages->setEnabled(true);
    }
    recentFiles().addFile(name);

    updatePhraseBookActions();
    updateStatistics();

    // Ensures that preview window is closed
    delete m_previewTool;

    m_contextView->setCurrentIndex(m_sortedContextsModel->index(0, 0));
}

RecentFiles& TrWindow::recentFiles()
{
    return m_recentFiles;
}

const QString& TrWindow::resourcePrefix()
{
    return m_resourcePrefix;
}

void TrWindow::open()
{
    if (maybeSave()) {
        openFile(pickTranslationFile());
    }
}

QString TrWindow::pickTranslationFile()
{
    QString dir;
    if (!recentFiles().isEmpty())
        dir = QFileInfo(recentFiles().lastOpenedFile()).absolutePath();

    return QFileDialog::getOpenFileName(this, tr("Open Translation File"), dir,
        tr("Qt translation sources (*.ts);;XLIFF localization files (*.xlf);;All files (*)"));
}

void TrWindow::save()
{
    if (m_filename.isEmpty())
        return;

    if (mainModel()->save(m_filename)) {
        updateCaption();
        statusBar()->showMessage(tr("File saved."), MessageMS);
        recentFiles().addFile(m_filename);
    } else {
        QMessageBox::warning(this, tr("Qt Linguist"), tr("Cannot save '%1'.").arg(m_filename));
    }
}

void TrWindow::saveAs()
{
    QString newFilename = QFileDialog::getSaveFileName(this, QString(), m_filename,
        tr( "Qt translation source (*.ts)\nXLIFF localization file (*.xlf)\nAll files (*)"));
    if (!newFilename.isEmpty()) {
        m_filename = newFilename;
        save();
        updateCaption();
    }
}

void TrWindow::releaseAs()
{
    QFileInfo oldFile(m_filename);
    QString newFilename = oldFile.absolutePath() + QLatin1String("/") + oldFile.completeBaseName() + QLatin1String(".qm");

    newFilename = QFileDialog::getSaveFileName(this, tr("Release"), newFilename,
        tr("Qt message files for released applications (*.qm)\nAll files (*)"));
    if (!newFilename.isEmpty()) {
        if (mainModel()->release(newFilename, false, false, Translator::Everything))
            statusBar()->showMessage(tr("File created."), MessageMS);
        else
            QMessageBox::warning(this, tr("Qt Linguist"), tr("Cannot save '%1'.").arg(newFilename));
    }
}

// No-question
void TrWindow::release()
{
    QFileInfo oldFile(m_filename);
    QString newFilename = oldFile.absolutePath() + QLatin1String("/") + oldFile.completeBaseName() + QLatin1String(".qm");

    if (!newFilename.isEmpty()) {
        if (mainModel()->release(newFilename, false, false, Translator::Everything))
            statusBar()->showMessage(tr("File created."), MessageMS);
        else
            QMessageBox::warning(this, tr("Qt Linguist"), tr("Cannot save '%1'.").arg(newFilename));
    }
}

void TrWindow::print()
{
    int pageNum = 0;
    QList <ContextItem *> ctxtList;
    QList <MessageItem *> msgList;
    const MessageItem *m;
    ContextItem *c;

    QPrintDialog dlg(&printer, this);
    if (dlg.exec()) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        printer.setDocName(m_filename);
        statusBar()->showMessage(tr("Printing..."));
        PrintOut pout(&printer);
        ctxtList = mainModel()->contextList();

        for (int i=0; i<ctxtList.count(); i++) {
            c = ctxtList.at(i);
            pout.vskip();
            pout.setRule(PrintOut::ThickRule);
            pout.setGuide(c->context());
            pout.addBox(100, tr("Context: %1").arg(c->context()),
                PrintOut::Strong);
            pout.flushLine();
            pout.addBox(4);
            pout.addBox(92, c->comment(), PrintOut::Emphasis);
            pout.flushLine();
            pout.setRule(PrintOut::ThickRule);

            msgList = c->messageItemList();
            for (int j=0; j<msgList.count(); j++) {
                m = msgList.at(j);
                pout.setRule(PrintOut::ThinRule);

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
                default:
                    type = QString(QLatin1String(""));
                }
                pout.addBox(40, m->sourceText());
                pout.addBox(4);
                if (m->message().isPlural() && mainModel()->language() != QLocale::C) {
                    QStringList transls = m->translations();
                    pout.addBox(40, transls.join(QLatin1String("\n")));
                } else {
                    pout.addBox(40, m->translation());
                }
                pout.addBox(4);
                pout.addBox(12, type, PrintOut::Normal, Qt::AlignRight);
                if (!m->comment().isEmpty()) {
                    pout.flushLine();
                    pout.addBox(4);
                    pout.addBox(92, m->comment(), PrintOut::Emphasis);
                }
                pout.flushLine(true);

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

void TrWindow::findAgain()
{
    if (mainModel()->contextsInList() <= 0)
        return;

    const QModelIndex &startIndex = m_messageView->currentIndex();
    QModelIndex index = nextMessage(startIndex);

    while (index.isValid()) {
        const QModelIndex &messageIndex = m_sortedMessagesModel->mapToSource(index);
        MessageItem *m = mainModel()->messageItem(messageIndex);

        switch (foundWhere) {
        case 0:
            foundWhere = FindDialog::SourceText;
            // fall-through to search source text
        case FindDialog::SourceText:
            if (searchItem(m->sourceText()))
                break;
            foundWhere = FindDialog::Translations;
            // fall-through to search translation
        case FindDialog::Translations:
            if (searchItem(m->translation()))
                break;
            foundWhere = FindDialog::Comments;
            // fall-through to search comment
        case FindDialog::Comments:
            if (searchItem(m->comment()))
                break;
            foundWhere = 0;
            // did not find the search string in this message
        }

        if (foundWhere != 0) {
            setCurrentMessage(messageIndex);

            // determine whether the search wrapped
            const QModelIndex &c1 = m_sortedContextsModel->mapFromSource(
                    m_sortedMessagesModel->mapToSource(startIndex)).parent();
            const QModelIndex &c2 = m_sortedContextsModel->mapFromSource(messageIndex).parent();
            const QModelIndex &m = m_sortedMessagesModel->mapFromSource(messageIndex);

            if (c2.row() < c1.row() || (c1.row() == c2.row() && m.row() <= startIndex.row()))
                statusBar()->showMessage(tr("Search wrapped."), MessageMS);

            m_findDialog->hide();
            return;
        }

        // since we don't search startIndex at the beginning, only now we have searched everything
        if (index == startIndex)
            break;

        index = nextMessage(index);
    }

    qApp->beep();
    QMessageBox::warning(m_findDialog, tr("Qt Linguist"),
                         QString( tr("Cannot find the string '%1'.") ).arg(findText));
    foundWhere  = 0;
}

void TrWindow::showTranslateDialog()
{
    m_translateDialog->show();
    m_translateDialog->activateWindow();
    m_translateDialog->raise();
}

void TrWindow::showBatchTranslateDialog()
{
    m_batchTranslateDialog->setPhraseBooks(m_phraseBooks);
    m_batchTranslateDialog->exec();
}

void TrWindow::translateAndFindNext(const QString& findWhat, const QString &translateTo,
                                    int matchOption, int mode, bool markFinished)
{
    findText = findWhat;
    m_translateTo = translateTo;
    findMatchCase = matchOption & TranslateDialog::MatchCase;
    m_markFinished = markFinished;
    m_findMatchSubstring = false;

    translate(mode);
}

void TrWindow::translate(int mode)
{
    int itemNo = 0;
    int scopeNo = 0;
    QModelIndex indxItem = currentMessageIndex();
    if (indxItem.isValid())
        itemNo = indxItem.row();      // The for-loop condition for the ContextItem will rule this potential overflow on itemNo
    QModelIndex indxScope = currentContextIndex();
    if (indxScope.isValid()) {
        scopeNo = indxScope.row();
    } else {
        scopeNo = itemNo;
        itemNo = 0;
    }

    int translatedCount = 0;
    bool found = false;

    MessageModel::iterator it = mainModel()->Iterator(scopeNo, itemNo);
    switch (mode) {
    case TranslateDialog::TranslateAll:
        {
            int passes = mainModel()->getMessageCount();
            while (passes > 0) {
                MessageItem *m = it.current();
                if (!m) {
                    it.reset();
                    m = it.current();
                }
                if (m && m->compare(findText, m_findMatchSubstring, findMatchCase ? Qt::CaseSensitive : Qt::CaseInsensitive)) {
                    mainModel()->setTranslation(it, m_translateTo);
                    if (indxScope.isValid() && it.contextNo() == scopeNo && it.messageNo() == itemNo)
                        me->setTranslation(m_translateTo, 0);
                    updateFinished(it.contextNo(), it.messageNo(), m_markFinished);
                    ++translatedCount;
                }
                ++it;
                --passes;
            }
            found = translatedCount == 0 ? false : true;
            if (found) {
                QMessageBox::warning(m_translateDialog, tr("Translate"),
                                  QString( tr("Translated %n entries to '%1'", 0, translatedCount).arg(m_translateTo) ));
            }
        }
        break;
    case TranslateDialog::Translate:
        {
            MessageItem *m = it.current();
            if (m && m->compare(findText, m_findMatchSubstring, findMatchCase ? Qt::CaseSensitive : Qt::CaseInsensitive)) {
                mainModel()->setTranslation(it, m_translateTo);
                updateFinished(it.contextNo(), it.messageNo(), m_markFinished);
                ++translatedCount;
            }
        }
    case TranslateDialog::Skip:
        {
            ++it;
            int passes = mainModel()->getMessageCount() - 1;
            while (passes > 0) {
                MessageItem *m = it.current();
                if (!m) {
                    it.reset();
                    m = it.current();
                }
                if (m && m->compare(findText, m_findMatchSubstring, findMatchCase ? Qt::CaseSensitive : Qt::CaseInsensitive)) {
                    found = true;
                    break;
                }
                ++it;
                --passes;
            }
        }
        break;
    }

    if (found) {
        QModelIndex cidx = mainModel()->index(it.contextNo(), 0);
        if (cidx.isValid()) {
            setCurrentMessage(mainModel()->index(it.messageNo(), 0, cidx));
        }
    } else {
        qApp->beep();
        QMessageBox::warning(m_translateDialog, tr("Qt Linguist"),
                             QString( tr("Cannot find the string '%1'.") ).arg(findText));
    }
}

bool TrWindow::searchItem(const QString &searchWhat)
{
    if ((findWhere & foundWhere) != 0) {
        QString text(searchWhat);

        if (findIgnoreAccelerators)
            text.remove(QLatin1Char('&'));

        const int foundOffset = text.indexOf(findText, 0,
                findMatchCase ? Qt::CaseSensitive : Qt::CaseInsensitive);

        if (foundOffset >= 0)
            return true;
    }
    return false;
}

MessageModel *TrWindow::mainModel() const
{
    return m_languagesManager->mainModel();
}

void TrWindow::newPhraseBook()
{
    QString name;
    for (;;) {
        name = QFileDialog::getSaveFileName(this, tr("Create New Phrase Book"),
            phraseBookDir, tr("Qt phrase books (*.qph)\nAll files (*)"));
        if (name.isEmpty())
            break;
        else if (!QFile::exists(name)) {
            break;
            QMessageBox::warning(this, tr("Qt Linguist"),
                tr("A file called '%1' already exists."
                "  Please choose another name.").arg(name));
        }
    }
    if (!name.isEmpty()) {
        PhraseBook pb;
        phraseBookDir = QFileInfo(name).absolutePath();
        if (savePhraseBook(name, pb)) {
            if (openPhraseBook(name))
                statusBar()->showMessage(tr("Phrase book created."), MessageMS);
        }
    }
}

bool TrWindow::phraseBooksContains(QString name)
{
    foreach(const PhraseBook *pb, m_phraseBooks) {
        if (pb->fileName() == name)
            return true;
    }

    return false;
}

void TrWindow::openPhraseBook()
{
    QString name = QFileDialog::getOpenFileName(this, tr("Open Phrase Book"),
    phraseBookDir, tr("Qt phrase books (*.qph);;All files (*)"));

    if (!name.isEmpty()) {
        phraseBookDir = QFileInfo(name).absolutePath();
        if (!phraseBooksContains(name)) {
            if (PhraseBook *phraseBook = openPhraseBook(name)) {
                int n = phraseBook->phrases().count();
                statusBar()->showMessage(tr("%n phrase(s) loaded.", 0, n), MessageMS);
            }
        }
    }
}

void TrWindow::closePhraseBook(QAction *action)
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
    disconnect(pb, SIGNAL(listChanged()), this, SLOT(updatePhraseDict()));
    updatePhraseDict();
    delete pb;
    updatePhraseBookActions();

    return;
}

void TrWindow::editPhraseBook(QAction *action)
{
    PhraseBook *pb = m_phraseBookMenu[PhraseEditMenu].value(action);
    PhraseBookBox box(pb, this);
    box.exec();

    updatePhraseDict();
}

void TrWindow::printPhraseBook(QAction *action)
{
    PhraseBook *phraseBook = m_phraseBookMenu[PhrasePrintMenu].value(action);

    int pageNum = 0;

    QPrintDialog dlg(&printer, this);
    if (dlg.exec()) {
        printer.setDocName(phraseBook->fileName());
        statusBar()->showMessage(tr("Printing..."));
        PrintOut pout(&printer);
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

void TrWindow::addToPhraseBook()
{
    QModelIndex currentIndex = m_sortedMessagesModel->mapToSource(m_messageView->currentIndex());
    MessageItem *currentMessage = mainModel()->messageItem(currentIndex);
    Phrase *phrase = new Phrase(currentMessage->sourceText(), currentMessage->translation(), QString());
    if (m_phraseBooks.size() == 1) {
        PhraseBook *book = m_phraseBooks.at(0);
        if (QMessageBox::information(this, tr("Add to phrase book"),
                              tr("Adding phrase to phrasebook %1").arg(book->friendlyPhraseBookName()),
                              QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok)
                              == QMessageBox::Ok)
            book->append(phrase);
    } else {
        QHash<QString, PhraseBook *> phraseBookHash;
        foreach (PhraseBook *book, m_phraseBooks)
            phraseBookHash.insert(book->friendlyPhraseBookName(), book);
        bool okPressed = false;
        QString selection = QInputDialog::getItem(this, tr("Add to phrase book"),
                                tr("Select phrase book to add to"),
                                phraseBookHash.keys(), 0, false, &okPressed);
        if (okPressed)
            phraseBookHash.value(selection)->append(phrase);
    }
}

void TrWindow::showLanguagesDialog()
{
    if (!m_languagesDialog)
        m_languagesDialog = new LanguagesDialog(m_languagesManager, this);

    m_languagesDialog->exec();
}

void TrWindow::resetSorting()
{
    m_contextView->sortByColumn(3, Qt::AscendingOrder);
    m_messageView->sortByColumn(3, Qt::AscendingOrder);
}

void TrWindow::manual()
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

void TrWindow::about()
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
                    "source) applications. Please see <tt>http://www.trolltech.com/company/model"
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
                    "<p>Copyright (C) 2000-2008 Trolltech ASA. All rights reserved."
                    "</p><p>The program is provided AS IS with NO WARRANTY OF ANY KIND,"
                    " INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A"
                    " PARTICULAR PURPOSE.</p>").arg(version).arg(edition));

    box.setWindowTitle(QApplication::translate("AboutDialog", "Qt Linguist"));
    box.setIcon(QMessageBox::NoIcon);
    box.exec();
}

void TrWindow::aboutQt()
{
    QMessageBox::aboutQt(this, tr("Qt Linguist"));
}

void TrWindow::setupPhrase()
{
    bool enabled = !m_phraseBooks.isEmpty();
    m_ui.menuClosePhraseBook->setEnabled(enabled);
    m_ui.menuEditPhraseBook->setEnabled(enabled);
    m_ui.menuPrintPhraseBook->setEnabled(enabled);
}

void TrWindow::closeEvent(QCloseEvent *e)
{
    if (maybeSave() && closePhraseBooks()) {
        e->accept();
        // Ensure that preview tool window is closed
        delete m_previewTool;
    } else {
        e->ignore();
    }
}

bool TrWindow::maybeSave()
{
    if (mainModel()->isModified()) {
        switch (QMessageBox::information(this, tr("Qt Linguist"),
            tr("Do you want to save '%1'?").arg(m_filename),
            QMessageBox::Yes | QMessageBox::Default,
            QMessageBox::No,
            QMessageBox::Cancel | QMessageBox::Escape))
        {
            case QMessageBox::Cancel:
                return false;
            case QMessageBox::Yes:
                save();
                return !mainModel()->isModified();
            case QMessageBox::No:
                break;
        }
    }
    return true;
}

void TrWindow::updateCaption()
{
    QString cap;
    bool enable = !m_filename.isEmpty();
    m_ui.actionSave->setEnabled(enable);
    m_ui.actionSaveAs->setEnabled(enable);
    m_ui.actionRelease->setEnabled(enable);
    m_ui.actionReleaseAs->setEnabled(enable);
    m_ui.actionPrint->setEnabled(enable);
    m_ui.actionAccelerators->setEnabled(enable);
    m_ui.actionEndingPunctuation->setEnabled(enable);
    m_ui.actionPhraseMatches->setEnabled(enable);
    m_ui.actionPlaceMarkerMatches->setEnabled(enable);
    m_ui.actionResetSorting->setEnabled(enable);

    if (m_filename.isEmpty())
        cap = tr("Qt Linguist by Trolltech[*]");
    else
        cap = tr("%1 - %2[*]").arg(tr("Qt Linguist by Trolltech")).arg(m_filename);
    setWindowTitle(cap);
}

void TrWindow::selectedContextChanged(const QModelIndex &sortedIndex, const QModelIndex &oldIndex)
{
    if (sortedIndex.isValid()) {
        setCurrentContext(m_sortedContextsModel->mapToSource(sortedIndex));
    } else if (oldIndex.isValid()) {
        m_contextView->setCurrentIndex(oldIndex);
    }
}

/*
 * Updates the message displayed in the message editor and related actions.
 */
void TrWindow::selectedMessageChanged(const QModelIndex &sortedIndex, const QModelIndex &oldIndex)
{
    // Keep a valid selection whenever possible
    if (!sortedIndex.isValid() && oldIndex.isValid()) {
        m_messageView->setCurrentIndex(oldIndex);
        return;
    }

    QModelIndex index = m_sortedMessagesModel->mapToSource(sortedIndex);

    MessageItem *m = 0;
    ContextItem *context = 0;

    if (index.isValid()) {
        m = mainModel()->messageItem(index);
        context = mainModel()->contextItem(index);
    }

    if (m) {
        me->showMessage(m, context);
        m_phraseView->setSourceText(m->sourceText());
        if (m->message().type() != TranslatorMessage::Obsolete) {
            m_sourceCodeView->setSourceContext(m->fileName(), m->lineNumber());
            m_ui.actionDoneAndNext->setEnabled(true);
            m_phraseView->setEnabled(true);
            m_errorsView->setEnabled(true);
        } else {
            m_sourceCodeView->setSourceContext(QString(), 0);
            m_ui.actionDoneAndNext->setEnabled(false);
            m_phraseView->setEnabled(false);
            m_errorsView->setEnabled(false);
        }
        danger(m, true);
    } else {
        me->showNothing();
        m_phraseView->setSourceText(QString());
        m_sourceCodeView->setSourceContext(QString(), 0);
        m_ui.actionDoneAndNext->setEnabled(false);
    }

    updatePhraseBookActions();
    m_ui.actionSelectAll->setEnabled(m_ui.actionDoneAndNext->isEnabled());
}

void TrWindow::refreshCurrentMessage()
{
    setCurrentMessage(m_sortedMessagesModel->mapToSource(m_messageView->currentIndex()));
}

void TrWindow::updateTranslation(const QStringList &translations)
{
    QModelIndex item = currentMessageIndex();
    if (!item.isValid())
        return;

    MessageItem *m = mainModel()->messageItem(item);
    if (m) {
        if (translations != m->translations()) {
            m->setTranslations(translations);
            updateDanger(m, true);
            mainModel()->updateItem(item);

            if (m->finished()) {
                updateFinished(false);
            } else {
                mainModel()->setModified(true);
            }
        }
    }
}

/**
 * Updates the translation in *both* the MessageModel and in the flat Translator 'model'.
 */
void TrWindow::updateTranslation(int context, int message, const QString &translation)
{
    MessageItem *m = mainModel()->messageItem(context, message);

    if (m && translation != m->translation()) {
        m->setTranslation(translation);

        updateDanger(m, true);

        if (m->finished())
            updateFinished(false);

        // Notify the view(s)
        QModelIndex idx = mainModel()->modelIndex(context, message);
        mainModel()->updateItem(idx);
    }
}

void TrWindow::updateFinished(int context, int message, bool finished)
{
    MessageItem *m = mainModel()->messageItem(context, message);
    if (finished != m->finished()) {
        m->setFinished(finished);
        updateProgress();
        updateStatistics();

        // Notify the view(s)
        QModelIndex idx = mainModel()->modelIndex(context, message);
        mainModel()->updateItem(idx);
    }
}

void TrWindow::updateFinished(bool finished)
{
    QModelIndex item = currentMessageIndex();
    if (!item.isValid())
        return;
    MessageItem *m = mainModel()->messageItem(item);

    if (finished != m->finished()) {
        m->setFinished(finished);
        updateProgress();
        updateStatistics();
        mainModel()->updateItem(item);
        mainModel()->updateItem(item.parent());
    }
}


void TrWindow::resetContextView()
{
    m_contextView->setCurrentIndex(m_sortedContextsModel->index(0, 0));
}

void TrWindow::doneAndNext()
{
    if (!currentMessageIndex().isValid())
        return;

    updateFinished(true);
    nextUnfinished();
}

void TrWindow::toggleFinished(const QModelIndex &index)
{
    if (!index.isValid() || index.column() != 0 || index.parent() == QModelIndex())
        return;

    MessageItem *m = mainModel()->messageItem(m_sortedMessagesModel->mapToSource(index));

    if (m->message().type() == TranslatorMessage::Obsolete)
        return;

    if (m->danger())
        printDanger(m);

    updateFinished(!m->finished());
}

int TrWindow::findCurrentContextRow()
{
    QModelIndex index = currentContextIndex();
    if (index.isValid())
        return index.row();

    //if no context is selected
    setCurrentContextRow(0);
    return 0;
}

/*
 * Receives a context index in the sorted messages model and returns the next logical context index
 * in the same model, based on the sort order of the contexts in the sorted contexts model.
 */
QModelIndex TrWindow::nextContext(const QModelIndex &index) const
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
QModelIndex TrWindow::prevContext(const QModelIndex &index) const
{
    QModelIndex sortedContextIndex = m_sortedContextsModel->mapFromSource(
            m_sortedMessagesModel->mapToSource(index));

    int prevRow = sortedContextIndex.row() - 1;
    if (prevRow < 0) prevRow = m_sortedContextsModel->rowCount() - 1;
    sortedContextIndex = m_sortedContextsModel->index(prevRow, index.column());

    return m_sortedMessagesModel->mapFromSource(
            m_sortedContextsModel->mapToSource(sortedContextIndex));
}

QModelIndex TrWindow::nextMessage(const QModelIndex &currentIndex, bool checkUnfinished) const
{
    bool found = false;
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
            idx = m_sortedMessagesModel->index(row, currentIndex.column(), par);
        } else {
            idx = m_sortedMessagesModel->index(row, currentIndex.column(), par);
        }
        MessageItem *m = mainModel()->messageItem(m_sortedMessagesModel->mapToSource(idx));
        if (m)
            found = checkUnfinished ? !(m->finished() || m->obsolete()) : true;
        if (idx == currentIndex) break;
    } while (!found);

    return (found) ? idx : QModelIndex();
}

QModelIndex TrWindow::prevMessage(const QModelIndex &currentIndex, bool checkUnfinished) const
{
    bool found = false;
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
            idx = m_sortedMessagesModel->index(row, currentIndex.column(), par);
        } else {
            idx = m_sortedMessagesModel->index(row, currentIndex.column(), par);
        }
        MessageItem *m = mainModel()->messageItem(m_sortedMessagesModel->mapToSource(idx));
        if (m)
            found = checkUnfinished ? !(m->finished() || m->obsolete()) : true;
        if (idx == currentIndex) break;
    } while (!found);

    return (found) ? idx : QModelIndex();
}

void TrWindow::nextUnfinished()
{
    if (m_ui.actionNextUnfinished->isEnabled()) {
        if (!next(true)) {
            // If no Unfinished message is left, the user has finished the job.  We
            // congratulate on a job well done with this ringing bell.
            statusBar()->showMessage(tr("No untranslated phrases left."), MessageMS);
            qApp->beep();
        }
    }
}

void TrWindow::prevUnfinished()
{
    if (m_ui.actionNextUnfinished->isEnabled()) {
        if (!prev(true)) {
            // If no Unfinished message is left, the user has finished the job.  We
            // congratulate on a job well done with this ringing bell.
            statusBar()->showMessage(tr("No untranslated phrases left."), MessageMS);
            qApp->beep();
        }
    }
}

void TrWindow::prev()
{
    prev(false);
}

void TrWindow::next()
{
    next(false);
}

bool TrWindow::prev(bool checkUnfinished)
{
    QModelIndex index = prevMessage(m_messageView->currentIndex(), checkUnfinished);
    if (index.isValid())
        setCurrentMessage(m_sortedMessagesModel->mapToSource(index));
    me->setEditorFocus();
    return index.isValid();
}

bool TrWindow::next(bool checkUnfinished)
{
    QModelIndex index = nextMessage(m_messageView->currentIndex(), checkUnfinished);
    if (index.isValid())
        setCurrentMessage(m_sortedMessagesModel->mapToSource(index));
    me->setEditorFocus();
    return index.isValid();
}

void TrWindow::findNext(const QString &text, int where, bool matchCase, bool ignoreAccelerators)
{
    if (text.isEmpty())
        return;
    findText = text;
    findWhere = where;
    findMatchCase = matchCase;
    findIgnoreAccelerators = ignoreAccelerators;
    m_ui.actionFindNext->setEnabled(true);
    findAgain();
}

void TrWindow::revalidate()
{
    if (mainModel()->contextsInList() <= 0)
        return;

    ContextItem *c;
    MessageItem *m;

    for (int ci = 0; ci < mainModel()->contextsInList(); ++ci) {
        c = mainModel()->contextItem(mainModel()->index(ci, 0));
        for (int mi = 0; mi < c->messageItemsInList(); ++mi) {
            m = c->messageItem(mi);
            updateDanger(m);
            //if (mainModel()->contextItem() == c)
            //    mainModel()->updateItem(mainModel()->index(mi, 0));
        }
        mainModel()->updateItem(mainModel()->index(ci, 0));
    }
}

QString TrWindow::friendlyString(const QString& str)
{
    QString f = str.toLower();
    f.replace(QRegExp(QString(QLatin1String("[.,:;!?()-]"))), QString(QLatin1String(" ")));
    f.remove(QLatin1Char('&'));
    return f.simplified();
}

void TrWindow::setupMenuBar()
{
    m_ui.actionAccelerators->setIcon(QIcon(m_resourcePrefix + QLatin1String("/accelerator.png")));
    m_ui.actionOpenPhraseBook->setIcon(QIcon(m_resourcePrefix + QLatin1String("/book.png")));
    m_ui.actionDoneAndNext->setIcon(QIcon(m_resourcePrefix + QLatin1String("/doneandnext.png")));
    m_ui.actionCopy->setIcon(QIcon(m_resourcePrefix + QLatin1String("/editcopy.png")));
    m_ui.actionCut->setIcon(QIcon(m_resourcePrefix + QLatin1String("/editcut.png")));
    m_ui.actionPaste->setIcon(QIcon(m_resourcePrefix + QLatin1String("/editpaste.png")));
    m_ui.actionOpen->setIcon(QIcon(m_resourcePrefix + QLatin1String("/fileopen.png")));
    m_ui.actionSave->setIcon(QIcon(m_resourcePrefix + QLatin1String("/filesave.png")));
    m_ui.actionNext->setIcon(QIcon(m_resourcePrefix + QLatin1String("/next.png")));
    m_ui.actionNextUnfinished->setIcon(QIcon(m_resourcePrefix + QLatin1String("/nextunfinished.png")));
    m_ui.actionPhraseMatches->setIcon(QIcon(m_resourcePrefix + QLatin1String("/phrase.png")));
    m_ui.actionEndingPunctuation->setIcon(QIcon(m_resourcePrefix + QLatin1String("/punctuation.png")));
    m_ui.actionPrev->setIcon(QIcon(m_resourcePrefix + QLatin1String("/prev.png")));
    m_ui.actionPrevUnfinished->setIcon(QIcon(m_resourcePrefix + QLatin1String("/prevunfinished.png")));
    m_ui.actionPrint->setIcon(QIcon(m_resourcePrefix + QLatin1String("/print.png")));
    m_ui.actionRedo->setIcon(QIcon(m_resourcePrefix + QLatin1String("/redo.png")));
    m_ui.actionFind->setIcon(QIcon(m_resourcePrefix + QLatin1String("/searchfind.png")));
    m_ui.actionUndo->setIcon(QIcon(m_resourcePrefix + QLatin1String("/undo.png")));
    m_ui.actionPlaceMarkerMatches->setIcon(QIcon(m_resourcePrefix + QLatin1String("/validateplacemarkers.png")));
    m_ui.actionWhatsThis->setIcon(QIcon(m_resourcePrefix + QLatin1String("/whatsthis.png")));

    // File menu
    connect(m_ui.actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(m_ui.actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(m_ui.actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(m_ui.actionRelease, SIGNAL(triggered()), this, SLOT(release()));
    connect(m_ui.actionReleaseAs, SIGNAL(triggered()), this, SLOT(releaseAs()));
    connect(m_ui.actionPrint, SIGNAL(triggered()), this, SLOT(print()));
    connect(m_ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));

    // Edit menu
    connect(m_ui.actionUndo, SIGNAL(triggered()), me, SLOT(undo()));
    connect(me, SIGNAL(undoAvailable(bool)), m_ui.actionUndo, SLOT(setEnabled(bool)));

    connect(m_ui.actionRedo, SIGNAL(triggered()), me, SLOT(redo()));
    connect(me, SIGNAL(redoAvailable(bool)), m_ui.actionRedo, SLOT(setEnabled(bool)));

    connect(m_ui.actionCopy, SIGNAL(triggered()), me, SLOT(copy()));
    connect(me, SIGNAL(copyAvailable(bool)), m_ui.actionCopy, SLOT(setEnabled(bool)));

    connect(me, SIGNAL(cutAvailable(bool)), m_ui.actionCut, SLOT(setEnabled(bool)));
    connect(m_ui.actionCut, SIGNAL(triggered()), me, SLOT(cut()));

    connect(me, SIGNAL(pasteAvailable(bool)), m_ui.actionPaste, SLOT(setEnabled(bool)));
    connect(m_ui.actionPaste, SIGNAL(triggered()), me, SLOT(paste()));

    connect(m_ui.actionSelectAll, SIGNAL(triggered()), me, SLOT(selectAll()));
    connect(m_ui.actionFind, SIGNAL(triggered()), m_findDialog, SLOT(find()));
    connect(m_ui.actionFindNext, SIGNAL(triggered()), this, SLOT(findAgain()));
    connect(m_ui.actionSearchAndTranslate, SIGNAL(triggered()), this, SLOT(showTranslateDialog()));
    connect(m_ui.actionBatchTranslation, SIGNAL(triggered()), this, SLOT(showBatchTranslateDialog()));

    connect( m_ui.actionTranslationFileSettings, SIGNAL(triggered()), this, SLOT(showTranslationSettings()) );

    // Translation menu
    // when updating the accelerators, remember the status bar
    connect(m_ui.actionPrevUnfinished, SIGNAL(triggered()), this, SLOT(prevUnfinished()));
    connect(m_ui.actionNextUnfinished, SIGNAL(triggered()), this, SLOT(nextUnfinished()));
    connect(m_ui.actionNext, SIGNAL(triggered()), this, SLOT(next()));
    connect(m_ui.actionPrev, SIGNAL(triggered()), this, SLOT(prev()));
    connect(m_ui.actionDoneAndNext, SIGNAL(triggered()), this, SLOT(doneAndNext()));
    connect(m_ui.actionBeginFromSource, SIGNAL(triggered()), me, SLOT(beginFromSource()));
    connect(me, SIGNAL(updateActions(bool)), m_ui.actionBeginFromSource, SLOT(setEnabled(bool)));

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
    connect(m_ui.actionOtherLanguages, SIGNAL(triggered()), this, SLOT(showLanguagesDialog()));

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
    m_ui.menuViewViews->addAction(m_sourceCodeDock->toggleViewAction());
    m_ui.menuViewViews->addAction(m_errorsDock->toggleViewAction());

    // Tools menu
    connect(m_batchTranslateDialog, SIGNAL(finished()), this, SLOT(finishedBatchTranslation()));
    connect(m_ui.actionPreviewForm, SIGNAL(triggered()), this, SLOT(previewForm()));

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

    connect(&recentFiles(), SIGNAL(changed()), this, SLOT(setupRecentFilesMenu()));
}

void TrWindow::updateViewMenu()
{
    if (stats)
        m_ui.actionStatistics->setChecked(stats->isVisible());
    else
        m_ui.actionStatistics->setChecked(false);
}

void TrWindow::showContextDock()
{
    m_contextDock->show();
    m_contextDock->raise();
}

void TrWindow::showMessagesDock()
{
    m_messagesDock->show();
    m_messagesDock->raise();
}

void TrWindow::showPhrasesDock()
{
    m_phrasesDock->show();
    m_phrasesDock->raise();
}

void TrWindow::showSourceCodeDock()
{
    m_sourceCodeDock->show();
    m_sourceCodeDock->raise();
}

void TrWindow::showErrorDock()
{
    m_errorsDock->show();
    m_errorsDock->raise();
}

void TrWindow::onWhatsThis()
{
    QWhatsThis::enterWhatsThisMode();
}

void TrWindow::setupToolBars()
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
    filet->addAction(m_ui.actionSave);
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

/*
 * Sets the current message to the first message in the given context, unless
 * the currently selected message already belongs to the new context.
 */
void TrWindow::setCurrentContext(const QModelIndex &sourceIndex)
{
    if (mainModel()->parent(currentMessageIndex()).row() != sourceIndex.row()) {
        const QModelIndex &index = m_sortedMessagesModel->mapFromSource(sourceIndex);
        const QModelIndex &firstChild = m_sortedMessagesModel->index(0, 0, index);
        setCurrentMessage(m_sortedMessagesModel->mapToSource(firstChild));
    }
}

void TrWindow::setCurrentContextRow(int row)
{
    setCurrentContext(mainModel()->index(row, 1));
}

/*
 * Updates the selected entries in the context and message views.
 */
void TrWindow::setCurrentMessage(const QModelIndex &index)
{
    const QModelIndex &contextIndex = mainModel()->parent(index);
    if (contextIndex.isValid()) {
        const QModelIndex &sortedContextIndex = m_sortedMessagesModel->mapFromSource(contextIndex);
        m_contextView->setCurrentIndex(m_sortedContextsModel->mapFromSource(contextIndex));
        if (m_messageView->rootIndex() != sortedContextIndex)
            m_messageView->setRootIndex(sortedContextIndex);
        m_messageView->setCurrentIndex(m_sortedMessagesModel->mapFromSource(index));
    }
}

QModelIndex TrWindow::currentContextIndex() const
{
    return m_sortedContextsModel->mapToSource(m_contextView->currentIndex());
}

QModelIndex TrWindow::currentMessageIndex() const
{
    return m_sortedMessagesModel->mapToSource(m_messageView->currentIndex());
}

PhraseBook *TrWindow::openPhraseBook(const QString& name)
{
    PhraseBook *pb = new PhraseBook();
    if (!pb->load(name)) {
        QMessageBox::warning(this, tr("Qt Linguist"),
            tr("Cannot read from phrase book '%1'.").arg(name));
        delete pb;
        return 0;
    }

    m_phraseBooks.append(pb);

    QAction *a = m_ui.menuClosePhraseBook->addAction(pb->friendlyPhraseBookName());
    m_phraseBookMenu[PhraseCloseMenu].insert(a, pb);
    a->setWhatsThis(tr("Close this phrase book."));

    a = m_ui.menuEditPhraseBook->addAction(pb->friendlyPhraseBookName());
    m_phraseBookMenu[PhraseEditMenu].insert(a, pb);
    a->setWhatsThis(tr("Allow you to add, modify, or delete"
        " phrases of this phrase book."));

    a = m_ui.menuPrintPhraseBook->addAction(pb->friendlyPhraseBookName());
    m_phraseBookMenu[PhrasePrintMenu].insert(a, pb);
    a->setWhatsThis(tr("Print the entries of the phrase"
        " book."));

    connect(pb, SIGNAL(listChanged()), this, SLOT(updatePhraseDict()));
    updatePhraseDict();
    updatePhraseBookActions();

    return pb;
}

bool TrWindow::savePhraseBook(QString &name, PhraseBook &pb)
{
    if (!name.contains(QLatin1String(".qph")) && !name.contains(QLatin1String(".")))
        name += QLatin1String(".qph");

    if (!pb.save(name)) {
        QMessageBox::warning(this, tr("Qt Linguist"),
            tr("Cannot create phrase book '%1'.").arg(name));
        return false;
    }
    return true;
}

bool TrWindow::maybeSavePhraseBook(PhraseBook *pb)
{
    if (pb->isModified())
        switch (QMessageBox::information(this, tr("Qt Linguist"),
            tr("Do you want to save phrasebook '%1'?").arg(pb->friendlyPhraseBookName()),
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

bool TrWindow::closePhraseBooks()
{
    foreach(PhraseBook *phraseBook, m_phraseBooks) {
        if (!maybeSavePhraseBook(phraseBook))
            return false;
    }
    return true;
}

void TrWindow::updateProgress()
{
    int numNonobsolete = mainModel()->getNumNonobsolete();
    int numFinished = mainModel()->getNumFinished();
    if (numNonobsolete == 0)
        progress->setText(QString(QLatin1String("    ")));
    else
        progress->setText(QString(QLatin1String(" %1/%2 ")).arg(numFinished)
        .arg(numNonobsolete));
    m_ui.actionPrevUnfinished->setEnabled(numFinished != numNonobsolete);
    m_ui.actionNextUnfinished->setEnabled(numFinished != numNonobsolete);

    m_ui.actionPrev->setEnabled(mainModel()->contextsInList() > 0);
    m_ui.actionNext->setEnabled(mainModel()->contextsInList() > 0);
}

void TrWindow::updatePhraseBookActions()
{
    bool phraseBookLoaded = !m_phraseBooks.isEmpty();
    m_ui.actionBatchTranslation->setEnabled(mainModel()->contextsInList() > 0 && phraseBookLoaded);
    m_ui.actionAddToPhraseBook->setEnabled(currentMessageIndex().isValid() && phraseBookLoaded);
}

void TrWindow::updatePhraseDict()
{
    m_phraseDict.clear();

    foreach (PhraseBook *pb, m_phraseBooks) {
        foreach (Phrase *p, pb->phrases()) {
            QString f = friendlyString(p->source());
            if (f.length() > 0) {
                f = f.split(QLatin1Char(' ')).first();
                if (!m_phraseDict.contains(f)) {
                    m_phraseDict.insert(f, QList<Phrase *>());
                }
                m_phraseDict[f].append(p);
            }
        }
    }
    revalidate();
    m_phraseView->update();
}

void TrWindow::printDanger(MessageItem *m)
{
    danger(m, true);
}

bool TrWindow::updateDanger(MessageItem *m, bool verbose)
{
    bool dngr = danger(m, verbose);

    if (dngr != m->danger())
        m->setDanger(dngr);

    return dngr;
}

bool TrWindow::danger(const MessageItem *m, bool verbose)
{
    QString source = m->sourceText();
    QStringList translations = m->translations();
    bool danger = false;
    m_errorsView->clear();

    if (m_ui.actionAccelerators->isChecked()) {
        bool sk = source.contains(Qt::Key_Ampersand);
        bool tk = true;
        for (int i = 0; i < translations.count() && tk; ++i) {
            tk &= bool(translations[i].contains(Qt::Key_Ampersand));
        }

        if (!sk && tk) {
            if (verbose)
                m_errorsView->addError(ErrorsView::SuperfluousAccelerator);
            danger = true;
        } else if (sk && !tk) {
            if (verbose)
                m_errorsView->addError(ErrorsView::MissingAccelerator);
            danger = true;
        }
    }
    if (m_ui.actionEndingPunctuation->isChecked()) {
        bool endingok = true;
        for (int i = 0; i < translations.count() && endingok; ++i) {
            endingok &= (ending(source, QLocale::C) == ending(translations[i], mainModel()->language()));
        }

        if (!endingok) {
            if (verbose)
                m_errorsView->addError(ErrorsView::PunctuationDiffer);
            danger = true;
        }
    }
    if (m_ui.actionPhraseMatches->isChecked()) {
        QString fsource = friendlyString(source);
        QString ftranslation = friendlyString(translations.first());
        QStringList lookupWords = fsource.split(QLatin1Char(' '));

        bool phraseFound;
        foreach (const QString &s, lookupWords) {
            if (m_phraseDict.contains(s)) {
                phraseFound = true;
                foreach (const Phrase *p, m_phraseDict.value(s)) {
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
                        m_errorsView->addError(ErrorsView::IgnoredPhrasebook, s);
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
        // When finished, all elements should have returned to a count of 0, if not there is a mismatch
        // between place markers in the source text and the translation text.
        QMap<int,int> placeMarkerIndexes;
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
                    while (c->isDigit()) ++c;
                    const QChar *escape_end = c;
                    bool ok = true;
                    int markerIndex = QString::fromRawData(escape_start, escape_end - escape_start).toInt(&ok);
                    if (ok) {
                        placeMarkerIndexes[markerIndex] += (pass == 0 ? numTranslations : -1);
                    }
                }
                ++c;
            }
        }

        foreach (int i, placeMarkerIndexes) {
            if (i != 0) {
                if (verbose)
                    m_errorsView->addError(ErrorsView::PlaceMarkersDiffer);
                danger = true;
                break;
            }
        }
    }

    QList<QString> errors = m_errorsView->errorList();
    if (!errors.isEmpty())
        statusBar()->showMessage(errors.at(0));
    else
        statusBar()->showMessage(QString());
    return danger;
}

void TrWindow::readConfig()
{
    QString keybase = SETTINGS_PREFIX;
    QSettings config;

    QRect r(pos(), size());
    restoreGeometry(config.value(keybase + QLatin1String("Geometry/WindowGeometry")).toByteArray());
    restoreState(config.value(keybase + QLatin1String("MainWindowState")).toByteArray());

    m_ui.actionAccelerators->setChecked(config.value(keybase + QLatin1String("Validators/Accelerator"), true).toBool());
    m_ui.actionEndingPunctuation->setChecked(config.value(keybase + QLatin1String("Validators/EndingPunctuation"), true).toBool());
    m_ui.actionPhraseMatches->setChecked(config.value(keybase + QLatin1String("Validators/PhraseMatch"), true).toBool());
    m_ui.actionPlaceMarkerMatches->setChecked(config.value(keybase + QLatin1String("Validators/PlaceMarkers"), true).toBool());

    recentFiles().readConfig();

    int size = config.beginReadArray(keybase + QLatin1String("OpenedPhraseBooks"));
    for (int i = 0; i < size; ++i) {
        config.setArrayIndex(i);
        openPhraseBook(config.value(QLatin1String("FileName")).toString());
    }
    config.endArray();
}

void TrWindow::writeConfig()
{
    QString keybase = SETTINGS_PREFIX;
    QSettings config;
    config.setValue(keybase + QLatin1String("Geometry/WindowGeometry"), saveGeometry());
    config.setValue(keybase + QLatin1String("Validators/Accelerator"), m_ui.actionAccelerators->isChecked());
    config.setValue(keybase + QLatin1String("Validators/EndingPunctuation"), m_ui.actionEndingPunctuation->isChecked());
    config.setValue(keybase + QLatin1String("Validators/PhraseMatch"), m_ui.actionPhraseMatches->isChecked());
    config.setValue(keybase + QLatin1String("Validators/PlaceMarkers"), m_ui.actionPlaceMarkerMatches->isChecked());
    config.setValue(keybase + QLatin1String("MainWindowState"), saveState());
    recentFiles().writeConfig();

    config.beginWriteArray(keybase + QLatin1String("OpenedPhraseBooks"), m_phraseBooks.size());
    for (int i = 0; i < m_phraseBooks.size(); ++i) {
        config.setArrayIndex(i);
        config.setValue(QLatin1String("FileName"), m_phraseBooks.at(i)->fileName());
    }
    config.endArray();
}

void TrWindow::setupRecentFilesMenu()
{
    m_ui.menuRecentlyOpenedFiles->clear();

    const QStringList recentFiles = m_recentFiles.filesList();
    if (!recentFiles.isEmpty()) {
        m_ui.menuRecentlyOpenedFiles->setEnabled(true);
        QStringList::ConstIterator it = recentFiles.begin();
        for (; it != recentFiles.end(); ++it) {
            m_ui.menuRecentlyOpenedFiles->addAction(*it);
        }
    } else {
        m_ui.menuRecentlyOpenedFiles->setEnabled(false);
    }
}

void TrWindow::recentFileActivated(QAction *action)
{
    if (!action->text().isEmpty()) {
        if (maybeSave())
            openFile(action->text());
    }
}

void TrWindow::toggleStatistics()
{
    if (m_ui.actionStatistics->isChecked()) {
        if (!stats) {
            stats = new Statistics(this);
            connect(mainModel(), SIGNAL(statsChanged(int,int,int,int,int,int)), stats,
                SLOT(updateStats(int,int,int,int,int,int)));
        }
        stats->show();
        updateStatistics();
    }
    else if (stats) {
        stats->close();
    }
}

void TrWindow::updateStatistics()
{
    // don't call this if stats dialog is not open
    // because this can be slow...
    if (!stats || !stats->isVisible())
        return;

    mainModel()->updateStatistics();
}

void TrWindow::finishedBatchTranslation()
{
    updateStatistics();
    updateProgress();
}

QStringList TrWindow::findFormFilesInCurrentTranslationFile()
{
    QStringList ret;
    for (MessageModel::iterator it = mainModel()->begin(); it != mainModel()->end(); ++it) {
        QString fileName = (*it)->message().fileName();
        if (fileName.endsWith(QLatin1String(".ui"))) {
            if (!QFileInfo(fileName).isAbsolute())
                fileName = QFileInfo(mainModel()->srcFileName()).absolutePath()
                                     + QLatin1Char('/') + fileName;
            if (!ret.contains(fileName)) {
                ret += fileName;
            }
        }
    }
    return ret;
}

void TrWindow::previewForm()
{
    if (m_previewTool.isNull()) {
        m_previewTool = new TrPreviewTool();
        QStringList formFiles = findFormFilesInCurrentTranslationFile();
        foreach (const QString &file, formFiles)
            m_previewTool->addFormFile(file);
        m_previewTool->addTranslator(mainModel()->translator(), QFileInfo(m_filename).fileName());
        m_previewTool->reloadTranslations();
        m_previewTool->cascade();
    } else {
        m_previewTool->reloadTranslations();
    }
    m_previewTool->show();
    m_previewTool->activateWindow();
    m_previewTool->raise();
}

void TrWindow::showTranslationSettings()
{
    if (!m_translationSettingsDialog) {
        m_translationSettingsDialog = new TranslationSettingsDialog(this);
    }
    m_translationSettingsDialog->setMessageModel(mainModel());
    m_translationSettingsDialog->exec();
}


QT_END_NAMESPACE
