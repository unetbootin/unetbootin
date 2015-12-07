/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "config.h"

#include <QPointer>
#include <QMap>

QT_BEGIN_NAMESPACE

class TabbedBrowser;
class HelpDialog;
class HelpWindow;
class QMenu;
class QDockWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    virtual ~MainWindow();

    TabbedBrowser *browsers() const;
    HelpDialog *helpDialog() const;

    void setupPopupMenu(QMenu *menu);
    static QString urlifyFileName(const QString &fileName);

    void removePendingBrowser(HelpWindow *win);

public slots:
    MainWindow *newWindow();

    void setup();
    void showLink(const QString &link);
    void showLinks(const QStringList &links);
    void saveSettings();
    void updateBookmarkMenu();
    void printingFinished();

private slots:
    void on_actionNewWindow_triggered();
    void on_actionGoHome_triggered();
    void on_actionFilePrint_triggered();
    void on_actionClose_triggered();
    void on_actionHelpWhatsThis_triggered();
    void on_actionHelpAssistant_triggered();
    void on_actionAboutApplication_triggered();
    void on_actionAboutAssistant_triggered();
    void on_actionSaveAs_triggered();
    void on_actionSyncToc_triggered();

    void about();
    void setupBookmarkMenu();
    void showBookmark(QAction *action);
    void showLinkFromClient(const QString &link);
    void showQtHelp();
    void showSearchLink(const QString &link, const QStringList &terms);
    void showGoActionLink();
    void updateProfileSettings();
    void backwardAvailable(bool);
    void forwardAvailable(bool);

    void browserTabChanged();
    void copyAvailable(bool yes);
    void updateTabActions(int index);
    void showFontSettingsDialog();

protected:
    void closeEvent(QCloseEvent *);
    void timerEvent(QTimerEvent *);

private:
    void setupGoActions();
    bool insertActionSeparator();
    void updateApplicationFontSettings(FontSettings &settings);

private:
    Ui::MainWindow ui;

    QList<QAction*> goActions;
    uint setupCompleted:1;
    TabbedBrowser *tabs;
    QMap<QAction*, QString> bookmarks;
    HelpDialog *helpDock;
    QDockWidget *dw;
    static QList<MainWindow*> windows;
    QMap<QAction*,QString> *goActionDocFiles;
    QList<QPair<HelpWindow*,QString> > pendingBrowsers;
};

#endif // MAINWINDOW_H

QT_END_NAMESPACE
