/*
*********************************************************************
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
*********************************************************************
*/

/********************************************************************************
** Form generated from reading ui file 'mainwindow.ui'
**
** Created: Thu Mar 5 20:53:50 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionExit;
    QAction *actionSave;
    QAction *actionSaveAs;
    QAction *actionRelease;
    QAction *actionPrint;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;
    QAction *actionSelectAll;
    QAction *actionFind;
    QAction *actionFindNext;
    QAction *actionPrevUnfinished;
    QAction *actionNextUnfinished;
    QAction *actionPrev;
    QAction *actionNext;
    QAction *actionDoneAndNext;
    QAction *actionBeginFromSource;
    QAction *actionAccelerators;
    QAction *actionEndingPunctuation;
    QAction *actionPhraseMatches;
    QAction *actionPlaceMarkerMatches;
    QAction *actionNewPhraseBook;
    QAction *actionOpenPhraseBook;
    QAction *actionResetSorting;
    QAction *actionDisplayGuesses;
    QAction *actionStatistics;
    QAction *actionManual;
    QAction *actionAbout;
    QAction *actionAboutQt;
    QAction *actionWhatsThis;
    QAction *actionSearchAndTranslate;
    QAction *actionBatchTranslation;
    QAction *actionReleaseAs;
    QAction *actionFile;
    QAction *actionEdit;
    QAction *actionTranslation;
    QAction *actionValidation;
    QAction *actionHelp;
    QAction *actionPreviewForm;
    QAction *actionTranslationFileSettings;
    QAction *actionAddToPhraseBook;
    QAction *actionOpenAux;
    QAction *actionSaveAll;
    QAction *actionReleaseAll;
    QAction *actionClose;
    QAction *actionCloseAll;
    QWidget *centralwidget;
    QMenuBar *menubar;
    QMenu *menuPhrases;
    QMenu *menuClosePhraseBook;
    QMenu *menuEditPhraseBook;
    QMenu *menuPrintPhraseBook;
    QMenu *menuValidation;
    QMenu *menuView;
    QMenu *menuViewViews;
    QMenu *menuToolbars;
    QMenu *menuHelp;
    QMenu *menuTranslation;
    QMenu *menuFile;
    QMenu *menuRecentlyOpenedFiles;
    QMenu *menuEdit;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(673, 461);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionExit->setMenuRole(QAction::QuitRole);
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        actionSaveAs = new QAction(MainWindow);
        actionSaveAs->setObjectName(QString::fromUtf8("actionSaveAs"));
        actionRelease = new QAction(MainWindow);
        actionRelease->setObjectName(QString::fromUtf8("actionRelease"));
        actionPrint = new QAction(MainWindow);
        actionPrint->setObjectName(QString::fromUtf8("actionPrint"));
        actionUndo = new QAction(MainWindow);
        actionUndo->setObjectName(QString::fromUtf8("actionUndo"));
        actionUndo->setEnabled(false);
        actionUndo->setMenuRole(QAction::NoRole);
        actionRedo = new QAction(MainWindow);
        actionRedo->setObjectName(QString::fromUtf8("actionRedo"));
        actionRedo->setEnabled(false);
        actionRedo->setMenuRole(QAction::NoRole);
        actionCut = new QAction(MainWindow);
        actionCut->setObjectName(QString::fromUtf8("actionCut"));
        actionCut->setEnabled(false);
        actionCut->setMenuRole(QAction::NoRole);
        actionCopy = new QAction(MainWindow);
        actionCopy->setObjectName(QString::fromUtf8("actionCopy"));
        actionCopy->setEnabled(false);
        actionCopy->setMenuRole(QAction::NoRole);
        actionPaste = new QAction(MainWindow);
        actionPaste->setObjectName(QString::fromUtf8("actionPaste"));
        actionPaste->setEnabled(false);
        actionPaste->setMenuRole(QAction::NoRole);
        actionSelectAll = new QAction(MainWindow);
        actionSelectAll->setObjectName(QString::fromUtf8("actionSelectAll"));
        actionSelectAll->setEnabled(false);
        actionSelectAll->setMenuRole(QAction::NoRole);
        actionFind = new QAction(MainWindow);
        actionFind->setObjectName(QString::fromUtf8("actionFind"));
        actionFind->setEnabled(false);
        actionFind->setMenuRole(QAction::NoRole);
        actionFindNext = new QAction(MainWindow);
        actionFindNext->setObjectName(QString::fromUtf8("actionFindNext"));
        actionFindNext->setEnabled(false);
        actionFindNext->setMenuRole(QAction::NoRole);
        actionPrevUnfinished = new QAction(MainWindow);
        actionPrevUnfinished->setObjectName(QString::fromUtf8("actionPrevUnfinished"));
        actionPrevUnfinished->setMenuRole(QAction::NoRole);
        actionNextUnfinished = new QAction(MainWindow);
        actionNextUnfinished->setObjectName(QString::fromUtf8("actionNextUnfinished"));
        actionNextUnfinished->setMenuRole(QAction::NoRole);
        actionPrev = new QAction(MainWindow);
        actionPrev->setObjectName(QString::fromUtf8("actionPrev"));
        actionPrev->setMenuRole(QAction::NoRole);
        actionNext = new QAction(MainWindow);
        actionNext->setObjectName(QString::fromUtf8("actionNext"));
        actionNext->setMenuRole(QAction::NoRole);
        actionDoneAndNext = new QAction(MainWindow);
        actionDoneAndNext->setObjectName(QString::fromUtf8("actionDoneAndNext"));
        actionDoneAndNext->setEnabled(false);
        actionDoneAndNext->setMenuRole(QAction::NoRole);
        actionBeginFromSource = new QAction(MainWindow);
        actionBeginFromSource->setObjectName(QString::fromUtf8("actionBeginFromSource"));
        actionBeginFromSource->setEnabled(false);
        actionBeginFromSource->setMenuRole(QAction::NoRole);
        actionAccelerators = new QAction(MainWindow);
        actionAccelerators->setObjectName(QString::fromUtf8("actionAccelerators"));
        actionAccelerators->setCheckable(true);
        actionAccelerators->setMenuRole(QAction::NoRole);
        actionEndingPunctuation = new QAction(MainWindow);
        actionEndingPunctuation->setObjectName(QString::fromUtf8("actionEndingPunctuation"));
        actionEndingPunctuation->setCheckable(true);
        actionEndingPunctuation->setMenuRole(QAction::NoRole);
        actionPhraseMatches = new QAction(MainWindow);
        actionPhraseMatches->setObjectName(QString::fromUtf8("actionPhraseMatches"));
        actionPhraseMatches->setCheckable(true);
        actionPhraseMatches->setMenuRole(QAction::NoRole);
        actionPlaceMarkerMatches = new QAction(MainWindow);
        actionPlaceMarkerMatches->setObjectName(QString::fromUtf8("actionPlaceMarkerMatches"));
        actionPlaceMarkerMatches->setCheckable(true);
        actionPlaceMarkerMatches->setMenuRole(QAction::NoRole);
        actionNewPhraseBook = new QAction(MainWindow);
        actionNewPhraseBook->setObjectName(QString::fromUtf8("actionNewPhraseBook"));
        actionNewPhraseBook->setMenuRole(QAction::NoRole);
        actionOpenPhraseBook = new QAction(MainWindow);
        actionOpenPhraseBook->setObjectName(QString::fromUtf8("actionOpenPhraseBook"));
        actionOpenPhraseBook->setMenuRole(QAction::NoRole);
        actionResetSorting = new QAction(MainWindow);
        actionResetSorting->setObjectName(QString::fromUtf8("actionResetSorting"));
        actionResetSorting->setCheckable(false);
        actionResetSorting->setChecked(false);
        actionResetSorting->setMenuRole(QAction::NoRole);
        actionDisplayGuesses = new QAction(MainWindow);
        actionDisplayGuesses->setObjectName(QString::fromUtf8("actionDisplayGuesses"));
        actionDisplayGuesses->setCheckable(true);
        actionDisplayGuesses->setChecked(true);
        actionDisplayGuesses->setMenuRole(QAction::NoRole);
        actionStatistics = new QAction(MainWindow);
        actionStatistics->setObjectName(QString::fromUtf8("actionStatistics"));
        actionStatistics->setCheckable(true);
        actionStatistics->setChecked(true);
        actionStatistics->setMenuRole(QAction::NoRole);
        actionManual = new QAction(MainWindow);
        actionManual->setObjectName(QString::fromUtf8("actionManual"));
        actionManual->setMenuRole(QAction::NoRole);
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionAbout->setMenuRole(QAction::AboutRole);
        actionAboutQt = new QAction(MainWindow);
        actionAboutQt->setObjectName(QString::fromUtf8("actionAboutQt"));
        actionAboutQt->setMenuRole(QAction::AboutQtRole);
        actionWhatsThis = new QAction(MainWindow);
        actionWhatsThis->setObjectName(QString::fromUtf8("actionWhatsThis"));
        actionWhatsThis->setCheckable(false);
        actionWhatsThis->setChecked(false);
        actionWhatsThis->setMenuRole(QAction::NoRole);
        actionSearchAndTranslate = new QAction(MainWindow);
        actionSearchAndTranslate->setObjectName(QString::fromUtf8("actionSearchAndTranslate"));
        actionSearchAndTranslate->setEnabled(false);
        actionSearchAndTranslate->setMenuRole(QAction::NoRole);
        actionBatchTranslation = new QAction(MainWindow);
        actionBatchTranslation->setObjectName(QString::fromUtf8("actionBatchTranslation"));
        actionBatchTranslation->setEnabled(false);
        actionBatchTranslation->setMenuRole(QAction::NoRole);
        actionReleaseAs = new QAction(MainWindow);
        actionReleaseAs->setObjectName(QString::fromUtf8("actionReleaseAs"));
        actionFile = new QAction(MainWindow);
        actionFile->setObjectName(QString::fromUtf8("actionFile"));
        actionFile->setCheckable(true);
        actionFile->setChecked(true);
        actionEdit = new QAction(MainWindow);
        actionEdit->setObjectName(QString::fromUtf8("actionEdit"));
        actionEdit->setCheckable(true);
        actionEdit->setChecked(true);
        actionTranslation = new QAction(MainWindow);
        actionTranslation->setObjectName(QString::fromUtf8("actionTranslation"));
        actionTranslation->setCheckable(true);
        actionTranslation->setChecked(true);
        actionValidation = new QAction(MainWindow);
        actionValidation->setObjectName(QString::fromUtf8("actionValidation"));
        actionValidation->setCheckable(true);
        actionValidation->setChecked(true);
        actionHelp = new QAction(MainWindow);
        actionHelp->setObjectName(QString::fromUtf8("actionHelp"));
        actionHelp->setCheckable(true);
        actionHelp->setChecked(true);
        actionPreviewForm = new QAction(MainWindow);
        actionPreviewForm->setObjectName(QString::fromUtf8("actionPreviewForm"));
        actionPreviewForm->setEnabled(false);
        actionPreviewForm->setMenuRole(QAction::NoRole);
        actionTranslationFileSettings = new QAction(MainWindow);
        actionTranslationFileSettings->setObjectName(QString::fromUtf8("actionTranslationFileSettings"));
        actionTranslationFileSettings->setEnabled(false);
        actionTranslationFileSettings->setMenuRole(QAction::NoRole);
        actionAddToPhraseBook = new QAction(MainWindow);
        actionAddToPhraseBook->setObjectName(QString::fromUtf8("actionAddToPhraseBook"));
        actionAddToPhraseBook->setEnabled(false);
        actionOpenAux = new QAction(MainWindow);
        actionOpenAux->setObjectName(QString::fromUtf8("actionOpenAux"));
        actionSaveAll = new QAction(MainWindow);
        actionSaveAll->setObjectName(QString::fromUtf8("actionSaveAll"));
        actionReleaseAll = new QAction(MainWindow);
        actionReleaseAll->setObjectName(QString::fromUtf8("actionReleaseAll"));
        actionClose = new QAction(MainWindow);
        actionClose->setObjectName(QString::fromUtf8("actionClose"));
        actionCloseAll = new QAction(MainWindow);
        actionCloseAll->setObjectName(QString::fromUtf8("actionCloseAll"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 673, 30));
        menuPhrases = new QMenu(menubar);
        menuPhrases->setObjectName(QString::fromUtf8("menuPhrases"));
        menuClosePhraseBook = new QMenu(menuPhrases);
        menuClosePhraseBook->setObjectName(QString::fromUtf8("menuClosePhraseBook"));
        menuEditPhraseBook = new QMenu(menuPhrases);
        menuEditPhraseBook->setObjectName(QString::fromUtf8("menuEditPhraseBook"));
        menuPrintPhraseBook = new QMenu(menuPhrases);
        menuPrintPhraseBook->setObjectName(QString::fromUtf8("menuPrintPhraseBook"));
        menuValidation = new QMenu(menubar);
        menuValidation->setObjectName(QString::fromUtf8("menuValidation"));
        menuView = new QMenu(menubar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        menuViewViews = new QMenu(menuView);
        menuViewViews->setObjectName(QString::fromUtf8("menuViewViews"));
        menuToolbars = new QMenu(menuView);
        menuToolbars->setObjectName(QString::fromUtf8("menuToolbars"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuTranslation = new QMenu(menubar);
        menuTranslation->setObjectName(QString::fromUtf8("menuTranslation"));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuRecentlyOpenedFiles = new QMenu(menuFile);
        menuRecentlyOpenedFiles->setObjectName(QString::fromUtf8("menuRecentlyOpenedFiles"));
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuTranslation->menuAction());
        menubar->addAction(menuValidation->menuAction());
        menubar->addAction(menuPhrases->menuAction());
        menubar->addAction(menuView->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuPhrases->addAction(actionNewPhraseBook);
        menuPhrases->addAction(actionOpenPhraseBook);
        menuPhrases->addAction(menuClosePhraseBook->menuAction());
        menuPhrases->addSeparator();
        menuPhrases->addAction(menuEditPhraseBook->menuAction());
        menuPhrases->addAction(menuPrintPhraseBook->menuAction());
        menuPhrases->addAction(actionAddToPhraseBook);
        menuValidation->addAction(actionAccelerators);
        menuValidation->addAction(actionEndingPunctuation);
        menuValidation->addAction(actionPhraseMatches);
        menuValidation->addAction(actionPlaceMarkerMatches);
        menuView->addAction(actionResetSorting);
        menuView->addAction(actionDisplayGuesses);
        menuView->addAction(actionStatistics);
        menuView->addSeparator();
        menuView->addAction(menuToolbars->menuAction());
        menuView->addAction(menuViewViews->menuAction());
        menuHelp->addAction(actionManual);
        menuHelp->addAction(actionAbout);
        menuHelp->addAction(actionAboutQt);
        menuHelp->addAction(actionWhatsThis);
        menuTranslation->addAction(actionPrevUnfinished);
        menuTranslation->addAction(actionNextUnfinished);
        menuTranslation->addAction(actionPrev);
        menuTranslation->addAction(actionNext);
        menuTranslation->addAction(actionDoneAndNext);
        menuTranslation->addAction(actionBeginFromSource);
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionOpenAux);
        menuFile->addAction(menuRecentlyOpenedFiles->menuAction());
        menuFile->addSeparator();
        menuFile->addAction(actionSaveAll);
        menuFile->addAction(actionSave);
        menuFile->addAction(actionSaveAs);
        menuFile->addAction(actionReleaseAll);
        menuFile->addAction(actionRelease);
        menuFile->addAction(actionReleaseAs);
        menuFile->addSeparator();
        menuFile->addAction(actionPrint);
        menuFile->addSeparator();
        menuFile->addAction(actionCloseAll);
        menuFile->addAction(actionClose);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuEdit->addAction(actionUndo);
        menuEdit->addAction(actionRedo);
        menuEdit->addSeparator();
        menuEdit->addAction(actionCut);
        menuEdit->addAction(actionCopy);
        menuEdit->addAction(actionPaste);
        menuEdit->addAction(actionSelectAll);
        menuEdit->addSeparator();
        menuEdit->addAction(actionFind);
        menuEdit->addAction(actionFindNext);
        menuEdit->addAction(actionSearchAndTranslate);
        menuEdit->addAction(actionBatchTranslation);
        menuEdit->addAction(actionTranslationFileSettings);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        actionOpen->setText(QApplication::translate("MainWindow", "&Open...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionOpen->setWhatsThis(QApplication::translate("MainWindow", "Open a Qt translation source file (TS file) for editing", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionOpen->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("MainWindow", "E&xit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionExit->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_WHATSTHIS
        actionExit->setWhatsThis(QApplication::translate("MainWindow", "Close this window and exit.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionExit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        actionSave->setText(QApplication::translate("MainWindow", "Save", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionSave->setWhatsThis(QApplication::translate("MainWindow", "Save changes made to this Qt translation source file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionSaveAs->setText(QApplication::translate("MainWindow", "Save &As...", 0, QApplication::UnicodeUTF8));
        actionSaveAs->setIconText(QApplication::translate("MainWindow", "Save As...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionSaveAs->setWhatsThis(QApplication::translate("MainWindow", "Save changes made to this Qt translation source file into a new file.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionRelease->setText(QApplication::translate("MainWindow", "Release", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionRelease->setWhatsThis(QApplication::translate("MainWindow", "Create a Qt message file suitable for released applications from the current message file.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionPrint->setText(QApplication::translate("MainWindow", "&Print...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionPrint->setWhatsThis(QApplication::translate("MainWindow", "Print a list of all the translation units in the current translation source file.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionPrint->setShortcut(QApplication::translate("MainWindow", "Ctrl+P", 0, QApplication::UnicodeUTF8));
        actionUndo->setText(QApplication::translate("MainWindow", "&Undo", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionUndo->setWhatsThis(QApplication::translate("MainWindow", "Undo the last editing operation performed on the current translation.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionUndo->setShortcut(QApplication::translate("MainWindow", "Ctrl+Z", 0, QApplication::UnicodeUTF8));
        actionRedo->setText(QApplication::translate("MainWindow", "&Redo", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionRedo->setWhatsThis(QApplication::translate("MainWindow", "Redo an undone editing operation performed on the translation.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionRedo->setShortcut(QApplication::translate("MainWindow", "Ctrl+Y", 0, QApplication::UnicodeUTF8));
        actionCut->setText(QApplication::translate("MainWindow", "Cu&t", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionCut->setWhatsThis(QApplication::translate("MainWindow", "Copy the selected translation text to the clipboard and deletes it.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionCut->setShortcut(QApplication::translate("MainWindow", "Ctrl+X", 0, QApplication::UnicodeUTF8));
        actionCopy->setText(QApplication::translate("MainWindow", "&Copy", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionCopy->setWhatsThis(QApplication::translate("MainWindow", "Copy the selected translation text to the clipboard.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionCopy->setShortcut(QApplication::translate("MainWindow", "Ctrl+C", 0, QApplication::UnicodeUTF8));
        actionPaste->setText(QApplication::translate("MainWindow", "&Paste", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionPaste->setWhatsThis(QApplication::translate("MainWindow", "Paste the clipboard text into the translation.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionPaste->setShortcut(QApplication::translate("MainWindow", "Ctrl+V", 0, QApplication::UnicodeUTF8));
        actionSelectAll->setText(QApplication::translate("MainWindow", "Select &All", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionSelectAll->setWhatsThis(QApplication::translate("MainWindow", "Select the whole translation text.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionSelectAll->setShortcut(QApplication::translate("MainWindow", "Ctrl+A", 0, QApplication::UnicodeUTF8));
        actionFind->setText(QApplication::translate("MainWindow", "&Find...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionFind->setWhatsThis(QApplication::translate("MainWindow", "Search for some text in the translation source file.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionFind->setShortcut(QApplication::translate("MainWindow", "Ctrl+F", 0, QApplication::UnicodeUTF8));
        actionFindNext->setText(QApplication::translate("MainWindow", "Find &Next", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionFindNext->setWhatsThis(QApplication::translate("MainWindow", "Continue the search where it was left.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionFindNext->setShortcut(QApplication::translate("MainWindow", "F3", 0, QApplication::UnicodeUTF8));
        actionPrevUnfinished->setText(QApplication::translate("MainWindow", "&Prev Unfinished", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionPrevUnfinished->setToolTip(QApplication::translate("MainWindow", "Previous unfinished item.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        actionPrevUnfinished->setWhatsThis(QApplication::translate("MainWindow", "Move to the previous unfinished item.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionPrevUnfinished->setShortcut(QApplication::translate("MainWindow", "Ctrl+K", 0, QApplication::UnicodeUTF8));
        actionNextUnfinished->setText(QApplication::translate("MainWindow", "&Next Unfinished", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionNextUnfinished->setToolTip(QApplication::translate("MainWindow", "Next unfinished item.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        actionNextUnfinished->setWhatsThis(QApplication::translate("MainWindow", "Move to the next unfinished item.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionNextUnfinished->setShortcut(QApplication::translate("MainWindow", "Ctrl+J", 0, QApplication::UnicodeUTF8));
        actionPrev->setText(QApplication::translate("MainWindow", "P&rev", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionPrev->setToolTip(QApplication::translate("MainWindow", "Move to previous item.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        actionPrev->setWhatsThis(QApplication::translate("MainWindow", "Move to the previous item.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionPrev->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+K", 0, QApplication::UnicodeUTF8));
        actionNext->setText(QApplication::translate("MainWindow", "Ne&xt", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionNext->setToolTip(QApplication::translate("MainWindow", "Next item.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        actionNext->setWhatsThis(QApplication::translate("MainWindow", "Move to the next item.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionNext->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+J", 0, QApplication::UnicodeUTF8));
        actionDoneAndNext->setText(QApplication::translate("MainWindow", "&Done and Next", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionDoneAndNext->setToolTip(QApplication::translate("MainWindow", "Mark item as done and move to the next unfinished item.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        actionDoneAndNext->setWhatsThis(QApplication::translate("MainWindow", "Mark this item as done and move to the next unfinished item.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionBeginFromSource->setText(QApplication::translate("MainWindow", "Copy from source text", 0, QApplication::UnicodeUTF8));
        actionBeginFromSource->setIconText(QApplication::translate("MainWindow", "Copy from source text", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionBeginFromSource->setToolTip(QApplication::translate("MainWindow", "Copies the source text into the translation field.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        actionBeginFromSource->setWhatsThis(QApplication::translate("MainWindow", "Copies the source text into the translation field.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionBeginFromSource->setShortcut(QApplication::translate("MainWindow", "Ctrl+B", 0, QApplication::UnicodeUTF8));
        actionAccelerators->setText(QApplication::translate("MainWindow", "&Accelerators", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionAccelerators->setToolTip(QApplication::translate("MainWindow", "Toggle the validity check of accelerators.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        actionAccelerators->setWhatsThis(QApplication::translate("MainWindow", "Toggle the validity check of accelerators, i.e. whether the number of ampersands in the source and translation text is the same. If the check fails, a message is shown in the warnings window.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionEndingPunctuation->setText(QApplication::translate("MainWindow", "&Ending Punctuation", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionEndingPunctuation->setToolTip(QApplication::translate("MainWindow", "Toggle the validity check of ending punctuation.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        actionEndingPunctuation->setWhatsThis(QApplication::translate("MainWindow", "Toggle the validity check of ending punctuation. If the check fails, a message is shown in the warnings window.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionPhraseMatches->setText(QApplication::translate("MainWindow", "&Phrase matches", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionPhraseMatches->setToolTip(QApplication::translate("MainWindow", "Toggle checking that phrase suggestions are used.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        actionPhraseMatches->setWhatsThis(QApplication::translate("MainWindow", "Toggle checking that phrase suggestions are used. If the check fails, a message is shown in the warnings window.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionPlaceMarkerMatches->setText(QApplication::translate("MainWindow", "Place &Marker Matches", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionPlaceMarkerMatches->setToolTip(QApplication::translate("MainWindow", "Toggle the validity check of place markers.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        actionPlaceMarkerMatches->setWhatsThis(QApplication::translate("MainWindow", "Toggle the validity check of place markers, i.e. whether %1, %2, ... are used consistently in the source text and translation text. If the check fails, a message is shown in the warnings window.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionNewPhraseBook->setText(QApplication::translate("MainWindow", "&New Phrase Book...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionNewPhraseBook->setWhatsThis(QApplication::translate("MainWindow", "Create a new phrase book.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionNewPhraseBook->setShortcut(QApplication::translate("MainWindow", "Ctrl+N", 0, QApplication::UnicodeUTF8));
        actionOpenPhraseBook->setText(QApplication::translate("MainWindow", "&Open Phrase Book...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionOpenPhraseBook->setWhatsThis(QApplication::translate("MainWindow", "Open a phrase book to assist translation.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionOpenPhraseBook->setShortcut(QApplication::translate("MainWindow", "Ctrl+H", 0, QApplication::UnicodeUTF8));
        actionResetSorting->setText(QApplication::translate("MainWindow", "&Reset Sorting", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionResetSorting->setWhatsThis(QApplication::translate("MainWindow", "Sort the items back in the same order as in the message file.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionDisplayGuesses->setText(QApplication::translate("MainWindow", "&Display guesses", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionDisplayGuesses->setWhatsThis(QApplication::translate("MainWindow", "Set whether or not to display translation guesses.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionStatistics->setText(QApplication::translate("MainWindow", "&Statistics", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionStatistics->setWhatsThis(QApplication::translate("MainWindow", "Display translation statistics.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionManual->setText(QApplication::translate("MainWindow", "&Manual", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionManual->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        actionManual->setShortcut(QApplication::translate("MainWindow", "F1", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("MainWindow", "About Qt Linguist", 0, QApplication::UnicodeUTF8));
        actionAboutQt->setText(QApplication::translate("MainWindow", "About Qt", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionAboutQt->setWhatsThis(QApplication::translate("MainWindow", "Display information about the Qt toolkit by Trolltech.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionWhatsThis->setText(QApplication::translate("MainWindow", "&What's This?", 0, QApplication::UnicodeUTF8));
        actionWhatsThis->setIconText(QApplication::translate("MainWindow", "What's This?", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionWhatsThis->setToolTip(QApplication::translate("MainWindow", "What's This?", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        actionWhatsThis->setWhatsThis(QApplication::translate("MainWindow", "Enter What's This? mode.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionWhatsThis->setShortcut(QApplication::translate("MainWindow", "Shift+F1", 0, QApplication::UnicodeUTF8));
        actionSearchAndTranslate->setText(QApplication::translate("MainWindow", "&Search And Translate...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionSearchAndTranslate->setWhatsThis(QApplication::translate("MainWindow", "Replace the translation on all entries that matches the search source text.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionBatchTranslation->setText(QApplication::translate("MainWindow", "&Batch Translation...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionBatchTranslation->setWhatsThis(QApplication::translate("MainWindow", "Batch translate all entries using the information in the phrase books.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionReleaseAs->setText(QApplication::translate("MainWindow", "Release As...", 0, QApplication::UnicodeUTF8));
        actionReleaseAs->setIconText(QApplication::translate("MainWindow", "Release As...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionReleaseAs->setWhatsThis(QApplication::translate("MainWindow", "Create a Qt message file suitable for released applications from the current message file. The filename will automatically be determined from the name of the .ts file.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionFile->setText(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
        actionEdit->setText(QApplication::translate("MainWindow", "Edit", 0, QApplication::UnicodeUTF8));
        actionTranslation->setText(QApplication::translate("MainWindow", "Translation", 0, QApplication::UnicodeUTF8));
        actionValidation->setText(QApplication::translate("MainWindow", "Validation", 0, QApplication::UnicodeUTF8));
        actionHelp->setText(QApplication::translate("MainWindow", "Help", 0, QApplication::UnicodeUTF8));
        actionPreviewForm->setText(QApplication::translate("MainWindow", "Open/Refresh Form &Preview", 0, QApplication::UnicodeUTF8));
        actionPreviewForm->setIconText(QApplication::translate("MainWindow", "Form Preview Tool", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionPreviewForm->setToolTip(QApplication::translate("MainWindow", "Form Preview Tool", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionPreviewForm->setShortcut(QApplication::translate("MainWindow", "F5", 0, QApplication::UnicodeUTF8));
        actionTranslationFileSettings->setText(QApplication::translate("MainWindow", "Translation File &Settings...", 0, QApplication::UnicodeUTF8));
        actionAddToPhraseBook->setText(QApplication::translate("MainWindow", "&Add to Phrase Book", 0, QApplication::UnicodeUTF8));
        actionAddToPhraseBook->setShortcut(QApplication::translate("MainWindow", "Ctrl+T", 0, QApplication::UnicodeUTF8));
        actionOpenAux->setText(QApplication::translate("MainWindow", "Open Read-O&nly...", 0, QApplication::UnicodeUTF8));
        actionSaveAll->setText(QApplication::translate("MainWindow", "&Save All", 0, QApplication::UnicodeUTF8));
        actionSaveAll->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", 0, QApplication::UnicodeUTF8));
        actionReleaseAll->setText(QApplication::translate("MainWindow", "&Release All", 0, QApplication::UnicodeUTF8));
        actionClose->setText(QApplication::translate("MainWindow", "Close", 0, QApplication::UnicodeUTF8));
        actionCloseAll->setText(QApplication::translate("MainWindow", "&Close All", 0, QApplication::UnicodeUTF8));
        actionCloseAll->setShortcut(QApplication::translate("MainWindow", "Ctrl+W", 0, QApplication::UnicodeUTF8));
        menuPhrases->setTitle(QApplication::translate("MainWindow", "&Phrases", 0, QApplication::UnicodeUTF8));
        menuClosePhraseBook->setTitle(QApplication::translate("MainWindow", "&Close Phrase Book", 0, QApplication::UnicodeUTF8));
        menuEditPhraseBook->setTitle(QApplication::translate("MainWindow", "&Edit Phrase Book", 0, QApplication::UnicodeUTF8));
        menuPrintPhraseBook->setTitle(QApplication::translate("MainWindow", "&Print Phrase Book", 0, QApplication::UnicodeUTF8));
        menuValidation->setTitle(QApplication::translate("MainWindow", "V&alidation", 0, QApplication::UnicodeUTF8));
        menuView->setTitle(QApplication::translate("MainWindow", "&View", 0, QApplication::UnicodeUTF8));
        menuViewViews->setTitle(QApplication::translate("MainWindow", "Vie&ws", 0, QApplication::UnicodeUTF8));
        menuToolbars->setTitle(QApplication::translate("MainWindow", "&Toolbars", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("MainWindow", "&Help", 0, QApplication::UnicodeUTF8));
        menuTranslation->setTitle(QApplication::translate("MainWindow", "&Translation", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
        menuRecentlyOpenedFiles->setTitle(QApplication::translate("MainWindow", "Recently Opened &Files", 0, QApplication::UnicodeUTF8));
        menuEdit->setTitle(QApplication::translate("MainWindow", "&Edit", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
