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

#ifndef TABBEDBROWSER_H
#define TABBEDBROWSER_H

#include "ui_tabbedbrowser.h"

QT_BEGIN_NAMESPACE

class MainWindow;
class HelpWindow;
class QStyleSheet;
class QMimeSourceFactory;
class QTimer;

class TabbedBrowser : public QWidget
{
    Q_OBJECT
public:
    TabbedBrowser(MainWindow *parent);
    virtual ~TabbedBrowser();

    MainWindow *mainWindow() const;
    HelpWindow *currentBrowser() const;
    QStringList sources() const;
    QList<HelpWindow*> browsers() const;

    HelpWindow* newBackgroundTab();
    HelpWindow* createHelpWindow();

    void setTitle(HelpWindow*, const QString &);

signals:
    void tabCountChanged(int count);
    void browserUrlChanged(const QString &link);

protected:
	void keyPressEvent(QKeyEvent *);
	bool eventFilter(QObject *o, QEvent *e);

public slots:
    void init();
    void forward();
    void backward();
    void setSource(const QString &ref);
    void reload();
    void home();
    void nextTab();
    void previousTab();
    void newTab(const QString &lnk);
    void zoomIn();
    void zoomOut();
    void updateTitle(const QString &title);
    void newTab();
    void transferFocus();
    void initHelpWindow(HelpWindow *win);
    void setup();
    void copy();
    void closeTab();
    void sourceChanged();

    void find();
    void findNext();
    void findPrevious();

private slots:
    void find(QString, bool forward = false, bool backward = false);
    void openTabMenu(const QPoint& pos);

private:
    Ui::TabbedBrowser ui;
    QWidget *lastCurrentTab;
    QFont tabFont;

    QString fixedFontFam;
    QColor lnkColor;
    bool underlineLnk;
	QTimer *autoHideTimer;
};

QT_END_NAMESPACE

#endif // TABBEDBROWSER_H
