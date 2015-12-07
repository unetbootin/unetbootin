/*
*********************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*********************************************************************
*/

/********************************************************************************
** Form generated from reading ui file 'mainwindow.ui'
**
** Created: Thu Mar 5 20:35:46 2009
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
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionFilePrint;
    QAction *actionFileExit;
    QAction *actionEditCopy;
    QAction *actionEditFind;
    QAction *actionEditFindNext;
    QAction *actionEditFindPrev;
    QAction *actionGoHome;
    QAction *actionGoPrevious;
    QAction *actionGoNext;
    QAction *actionAboutAssistant;
    QAction *actionAboutApplication;
    QAction *actionZoomIn;
    QAction *actionZoomOut;
    QAction *actionNewWindow;
    QAction *actionClose;
    QAction *actionAddBookmark;
    QAction *actionHelpWhatsThis;
    QAction *actionOpenPage;
    QAction *actionNextPage;
    QAction *actionPrevPage;
    QAction *actionClosePage;
    QAction *actionHelpAssistant;
    QAction *actionSaveAs;
    QAction *actionSyncToc;
    QAction *actionEditFont_Settings;
    QWidget *__qt_central_widget;
    QToolBar *Toolbar;
    QToolBar *goActionToolbar;
    QMenuBar *menubar;
    QMenu *helpMenu;
    QMenu *fileMenu;
    QMenu *bookmarkMenu;
    QMenu *goMenu;
    QMenu *viewMenu;
    QMenu *editMenu;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(949, 670);
        actionFilePrint = new QAction(MainWindow);
        actionFilePrint->setObjectName(QString::fromUtf8("actionFilePrint"));
        const QIcon icon = QIcon(QString::fromUtf8(":/trolltech/assistant/images/win/print.png"));
        actionFilePrint->setIcon(icon);
        actionFileExit = new QAction(MainWindow);
        actionFileExit->setObjectName(QString::fromUtf8("actionFileExit"));
        actionFileExit->setMenuRole(QAction::QuitRole);
        actionEditCopy = new QAction(MainWindow);
        actionEditCopy->setObjectName(QString::fromUtf8("actionEditCopy"));
        const QIcon icon1 = QIcon(QString::fromUtf8(":/trolltech/assistant/images/win/editcopy.png"));
        actionEditCopy->setIcon(icon1);
        actionEditFind = new QAction(MainWindow);
        actionEditFind->setObjectName(QString::fromUtf8("actionEditFind"));
        const QIcon icon2 = QIcon(QString::fromUtf8(":/trolltech/assistant/images/win/find.png"));
        actionEditFind->setIcon(icon2);
        actionEditFindNext = new QAction(MainWindow);
        actionEditFindNext->setObjectName(QString::fromUtf8("actionEditFindNext"));
        actionEditFindPrev = new QAction(MainWindow);
        actionEditFindPrev->setObjectName(QString::fromUtf8("actionEditFindPrev"));
        actionGoHome = new QAction(MainWindow);
        actionGoHome->setObjectName(QString::fromUtf8("actionGoHome"));
        const QIcon icon3 = QIcon(QString::fromUtf8(":/trolltech/assistant/images/win/home.png"));
        actionGoHome->setIcon(icon3);
        actionGoPrevious = new QAction(MainWindow);
        actionGoPrevious->setObjectName(QString::fromUtf8("actionGoPrevious"));
        const QIcon icon4 = QIcon(QString::fromUtf8(":/trolltech/assistant/images/win/previous.png"));
        actionGoPrevious->setIcon(icon4);
        actionGoNext = new QAction(MainWindow);
        actionGoNext->setObjectName(QString::fromUtf8("actionGoNext"));
        const QIcon icon5 = QIcon(QString::fromUtf8(":/trolltech/assistant/images/win/next.png"));
        actionGoNext->setIcon(icon5);
        actionAboutAssistant = new QAction(MainWindow);
        actionAboutAssistant->setObjectName(QString::fromUtf8("actionAboutAssistant"));
        actionAboutAssistant->setMenuRole(QAction::AboutRole);
        actionAboutApplication = new QAction(MainWindow);
        actionAboutApplication->setObjectName(QString::fromUtf8("actionAboutApplication"));
        actionAboutApplication->setMenuRole(QAction::AboutQtRole);
        actionZoomIn = new QAction(MainWindow);
        actionZoomIn->setObjectName(QString::fromUtf8("actionZoomIn"));
        const QIcon icon6 = QIcon(QString::fromUtf8(":/trolltech/assistant/images/win/zoomin.png"));
        actionZoomIn->setIcon(icon6);
        actionZoomOut = new QAction(MainWindow);
        actionZoomOut->setObjectName(QString::fromUtf8("actionZoomOut"));
        const QIcon icon7 = QIcon(QString::fromUtf8(":/trolltech/assistant/images/win/zoomout.png"));
        actionZoomOut->setIcon(icon7);
        actionNewWindow = new QAction(MainWindow);
        actionNewWindow->setObjectName(QString::fromUtf8("actionNewWindow"));
        actionClose = new QAction(MainWindow);
        actionClose->setObjectName(QString::fromUtf8("actionClose"));
        actionAddBookmark = new QAction(MainWindow);
        actionAddBookmark->setObjectName(QString::fromUtf8("actionAddBookmark"));
        actionHelpWhatsThis = new QAction(MainWindow);
        actionHelpWhatsThis->setObjectName(QString::fromUtf8("actionHelpWhatsThis"));
        const QIcon icon8 = QIcon(QString::fromUtf8(":/trolltech/assistant/images/win/whatsthis.png"));
        actionHelpWhatsThis->setIcon(icon8);
        actionOpenPage = new QAction(MainWindow);
        actionOpenPage->setObjectName(QString::fromUtf8("actionOpenPage"));
        actionNextPage = new QAction(MainWindow);
        actionNextPage->setObjectName(QString::fromUtf8("actionNextPage"));
        actionPrevPage = new QAction(MainWindow);
        actionPrevPage->setObjectName(QString::fromUtf8("actionPrevPage"));
        actionClosePage = new QAction(MainWindow);
        actionClosePage->setObjectName(QString::fromUtf8("actionClosePage"));
        actionHelpAssistant = new QAction(MainWindow);
        actionHelpAssistant->setObjectName(QString::fromUtf8("actionHelpAssistant"));
        const QIcon icon9 = QIcon(QString::fromUtf8(":/trolltech/assistant/images/assistant.png"));
        actionHelpAssistant->setIcon(icon9);
        actionSaveAs = new QAction(MainWindow);
        actionSaveAs->setObjectName(QString::fromUtf8("actionSaveAs"));
        actionSyncToc = new QAction(MainWindow);
        actionSyncToc->setObjectName(QString::fromUtf8("actionSyncToc"));
        const QIcon icon10 = QIcon(QString::fromUtf8(":/trolltech/assistant/images/win/synctoc.png"));
        actionSyncToc->setIcon(icon10);
        actionEditFont_Settings = new QAction(MainWindow);
        actionEditFont_Settings->setObjectName(QString::fromUtf8("actionEditFont_Settings"));
        actionEditFont_Settings->setMenuRole(QAction::PreferencesRole);
        __qt_central_widget = new QWidget(MainWindow);
        __qt_central_widget->setObjectName(QString::fromUtf8("__qt_central_widget"));
        MainWindow->setCentralWidget(__qt_central_widget);
        Toolbar = new QToolBar(MainWindow);
        Toolbar->setObjectName(QString::fromUtf8("Toolbar"));
        Toolbar->setOrientation(Qt::Horizontal);
        MainWindow->addToolBar(Qt::TopToolBarArea, Toolbar);
        goActionToolbar = new QToolBar(MainWindow);
        goActionToolbar->setObjectName(QString::fromUtf8("goActionToolbar"));
        goActionToolbar->setOrientation(Qt::Horizontal);
        MainWindow->addToolBar(Qt::TopToolBarArea, goActionToolbar);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 949, 29));
        helpMenu = new QMenu(menubar);
        helpMenu->setObjectName(QString::fromUtf8("helpMenu"));
        fileMenu = new QMenu(menubar);
        fileMenu->setObjectName(QString::fromUtf8("fileMenu"));
        bookmarkMenu = new QMenu(menubar);
        bookmarkMenu->setObjectName(QString::fromUtf8("bookmarkMenu"));
        goMenu = new QMenu(menubar);
        goMenu->setObjectName(QString::fromUtf8("goMenu"));
        viewMenu = new QMenu(menubar);
        viewMenu->setObjectName(QString::fromUtf8("viewMenu"));
        editMenu = new QMenu(menubar);
        editMenu->setObjectName(QString::fromUtf8("editMenu"));
        MainWindow->setMenuBar(menubar);

        Toolbar->addAction(actionGoPrevious);
        Toolbar->addAction(actionGoNext);
        Toolbar->addAction(actionGoHome);
        Toolbar->addAction(actionSyncToc);
        Toolbar->addSeparator();
        Toolbar->addAction(actionEditCopy);
        Toolbar->addAction(actionEditFind);
        Toolbar->addAction(actionFilePrint);
        Toolbar->addSeparator();
        Toolbar->addAction(actionZoomIn);
        Toolbar->addAction(actionZoomOut);
        Toolbar->addSeparator();
        Toolbar->addAction(actionHelpWhatsThis);
        menubar->addAction(fileMenu->menuAction());
        menubar->addAction(editMenu->menuAction());
        menubar->addAction(viewMenu->menuAction());
        menubar->addAction(goMenu->menuAction());
        menubar->addAction(bookmarkMenu->menuAction());
        menubar->addAction(helpMenu->menuAction());
        helpMenu->addAction(actionHelpAssistant);
        helpMenu->addSeparator();
        helpMenu->addAction(actionAboutAssistant);
        helpMenu->addAction(actionAboutApplication);
        helpMenu->addSeparator();
        helpMenu->addAction(actionHelpWhatsThis);
        fileMenu->addAction(actionNewWindow);
        fileMenu->addAction(actionOpenPage);
        fileMenu->addAction(actionClosePage);
        fileMenu->addSeparator();
        fileMenu->addAction(actionSaveAs);
        fileMenu->addSeparator();
        fileMenu->addAction(actionFilePrint);
        fileMenu->addSeparator();
        fileMenu->addAction(actionClose);
        fileMenu->addAction(actionFileExit);
        goMenu->addAction(actionGoPrevious);
        goMenu->addAction(actionGoNext);
        goMenu->addAction(actionGoHome);
        goMenu->addAction(actionSyncToc);
        goMenu->addSeparator();
        goMenu->addAction(actionNextPage);
        goMenu->addAction(actionPrevPage);
        viewMenu->addAction(actionZoomIn);
        viewMenu->addAction(actionZoomOut);
        editMenu->addAction(actionEditCopy);
        editMenu->addAction(actionEditFind);
        editMenu->addAction(actionEditFindNext);
        editMenu->addAction(actionEditFindPrev);
        editMenu->addSeparator();
        editMenu->addAction(actionEditFont_Settings);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Qt Assistant by Nokia", 0, QApplication::UnicodeUTF8));
        actionFilePrint->setText(QApplication::translate("MainWindow", "&Print...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionFilePrint->setWhatsThis(QApplication::translate("MainWindow", "Print the currently displayed page.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionFilePrint->setShortcut(QApplication::translate("MainWindow", "Ctrl+P", 0, QApplication::UnicodeUTF8));
        actionFileExit->setText(QApplication::translate("MainWindow", "E&xit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionFileExit->setWhatsThis(QApplication::translate("MainWindow", "Quit Qt Assistant.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionFileExit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        actionEditCopy->setText(QApplication::translate("MainWindow", "&Copy", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionEditCopy->setWhatsThis(QApplication::translate("MainWindow", "Copy the selected text to the clipboard.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionEditCopy->setShortcut(QApplication::translate("MainWindow", "Ctrl+C", 0, QApplication::UnicodeUTF8));
        actionEditFind->setText(QApplication::translate("MainWindow", "&Find in Text...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionEditFind->setWhatsThis(QApplication::translate("MainWindow", "Open the Find dialog. Qt Assistant will search the currently displayed page for the text you enter.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionEditFind->setShortcut(QApplication::translate("MainWindow", "Ctrl+F", 0, QApplication::UnicodeUTF8));
        actionEditFindNext->setText(QApplication::translate("MainWindow", "Find &Next", 0, QApplication::UnicodeUTF8));
        actionEditFindNext->setShortcut(QApplication::translate("MainWindow", "F3", 0, QApplication::UnicodeUTF8));
        actionEditFindPrev->setText(QApplication::translate("MainWindow", "Find &Previous", 0, QApplication::UnicodeUTF8));
        actionEditFindPrev->setShortcut(QApplication::translate("MainWindow", "Shift+F3", 0, QApplication::UnicodeUTF8));
        actionGoHome->setText(QApplication::translate("MainWindow", "&Home", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionGoHome->setWhatsThis(QApplication::translate("MainWindow", "Go to the home page. Qt Assistant's home page is the Qt Reference Documentation.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionGoHome->setShortcut(QApplication::translate("MainWindow", "Ctrl+Home", 0, QApplication::UnicodeUTF8));
        actionGoPrevious->setText(QApplication::translate("MainWindow", "&Previous", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionGoPrevious->setWhatsThis(QApplication::translate("MainWindow", "Go to the previous page.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionGoPrevious->setShortcut(QApplication::translate("MainWindow", "Alt+Left", 0, QApplication::UnicodeUTF8));
        actionGoNext->setText(QApplication::translate("MainWindow", "&Next", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionGoNext->setWhatsThis(QApplication::translate("MainWindow", "Go to the next page.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionGoNext->setShortcut(QApplication::translate("MainWindow", "Alt+Right", 0, QApplication::UnicodeUTF8));
        actionAboutAssistant->setText(QApplication::translate("MainWindow", "About Qt Assistant", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionAboutAssistant->setWhatsThis(QApplication::translate("MainWindow", "Display further information about Qt Assistant.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionAboutApplication->setText(QApplication::translate("MainWindow", "About Qt", 0, QApplication::UnicodeUTF8));
        actionZoomIn->setText(QApplication::translate("MainWindow", "Zoom &in", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionZoomIn->setWhatsThis(QApplication::translate("MainWindow", "Zoom in on the document, i.e. increase the font size.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionZoomIn->setShortcut(QApplication::translate("MainWindow", "Ctrl++", 0, QApplication::UnicodeUTF8));
        actionZoomOut->setText(QApplication::translate("MainWindow", "Zoom &out", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionZoomOut->setWhatsThis(QApplication::translate("MainWindow", "Zoom out on the document, i.e. decrease the font size.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionZoomOut->setShortcut(QApplication::translate("MainWindow", "Ctrl+-", 0, QApplication::UnicodeUTF8));
        actionNewWindow->setText(QApplication::translate("MainWindow", "New Window", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionNewWindow->setWhatsThis(QApplication::translate("MainWindow", "Open a new window.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionNewWindow->setShortcut(QApplication::translate("MainWindow", "Ctrl+N", 0, QApplication::UnicodeUTF8));
        actionClose->setText(QApplication::translate("MainWindow", "&Close", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionClose->setWhatsThis(QApplication::translate("MainWindow", "Close the current window.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionClose->setShortcut(QApplication::translate("MainWindow", "Ctrl+W", 0, QApplication::UnicodeUTF8));
        actionAddBookmark->setText(QApplication::translate("MainWindow", "&Add Bookmark", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionAddBookmark->setWhatsThis(QApplication::translate("MainWindow", "Add the currently displayed page as a new bookmark.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionHelpWhatsThis->setText(QApplication::translate("MainWindow", "What's This?", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionHelpWhatsThis->setStatusTip(QApplication::translate("MainWindow", "\"What's This?\" context sensitive help.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_WHATSTHIS
        actionHelpWhatsThis->setWhatsThis(QApplication::translate("MainWindow", "\"What's This?\" context sensitive help.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionHelpWhatsThis->setShortcut(QApplication::translate("MainWindow", "Shift+F1", 0, QApplication::UnicodeUTF8));
        actionOpenPage->setText(QApplication::translate("MainWindow", "Add Tab", 0, QApplication::UnicodeUTF8));
        actionOpenPage->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+N", 0, QApplication::UnicodeUTF8));
        actionNextPage->setText(QApplication::translate("MainWindow", "Next Tab", 0, QApplication::UnicodeUTF8));
        actionNextPage->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+Right", 0, QApplication::UnicodeUTF8));
        actionPrevPage->setText(QApplication::translate("MainWindow", "Previous Tab", 0, QApplication::UnicodeUTF8));
        actionPrevPage->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+Left", 0, QApplication::UnicodeUTF8));
        actionClosePage->setText(QApplication::translate("MainWindow", "Close Tab", 0, QApplication::UnicodeUTF8));
        actionClosePage->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+Q", 0, QApplication::UnicodeUTF8));
        actionHelpAssistant->setText(QApplication::translate("MainWindow", "Qt Assistant Manual", 0, QApplication::UnicodeUTF8));
        actionHelpAssistant->setShortcut(QApplication::translate("MainWindow", "F1", 0, QApplication::UnicodeUTF8));
        actionSaveAs->setText(QApplication::translate("MainWindow", "Save Page As...", 0, QApplication::UnicodeUTF8));
        actionSaveAs->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+S", 0, QApplication::UnicodeUTF8));
        actionSyncToc->setText(QApplication::translate("MainWindow", "Sync with Table of Contents", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        actionSyncToc->setWhatsThis(QApplication::translate("MainWindow", "Select the page in contents tab.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        actionEditFont_Settings->setText(QApplication::translate("MainWindow", "Font Settings...", 0, QApplication::UnicodeUTF8));
        Toolbar->setWindowTitle(QApplication::translate("MainWindow", "Toolbar", 0, QApplication::UnicodeUTF8));
        goActionToolbar->setWindowTitle(QApplication::translate("MainWindow", "Go", 0, QApplication::UnicodeUTF8));
        helpMenu->setTitle(QApplication::translate("MainWindow", "&Help", 0, QApplication::UnicodeUTF8));
        fileMenu->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
        bookmarkMenu->setTitle(QApplication::translate("MainWindow", "Boo&kmarks", 0, QApplication::UnicodeUTF8));
        goMenu->setTitle(QApplication::translate("MainWindow", "&Go", 0, QApplication::UnicodeUTF8));
        viewMenu->setTitle(QApplication::translate("MainWindow", "&View", 0, QApplication::UnicodeUTF8));
        editMenu->setTitle(QApplication::translate("MainWindow", "&Edit", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
