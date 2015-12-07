/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
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
