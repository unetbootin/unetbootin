/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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

#ifndef QDBUSVIEWER_H
#define QDBUSVIEWER_H

#include <QtGui/QtGui>
#include <QtDBus/QtDBus>

QT_FORWARD_DECLARE_CLASS(QTreeView);
QT_FORWARD_DECLARE_CLASS(QDomDocument);
QT_FORWARD_DECLARE_CLASS(QDomElement);

struct BusSignature
{
    QString mService, mPath, mInterface, mName;
};

class QDBusViewer: public QWidget
{
    Q_OBJECT
public:
    QDBusViewer(const QDBusConnection &connection, QWidget *parent = 0);

public slots:
    void refresh();
    void about();

private slots:
    void serviceChanged(QTreeWidgetItem *item);
    void showContextMenu(const QPoint &);
    void connectionRequested(const BusSignature &sig);
    void callMethod(const BusSignature &sig);
    void getProperty(const BusSignature &sig);
    void setProperty(const BusSignature &sig);
    void dumpMessage(const QDBusMessage &msg);
    void refreshChildren();

    void serviceRegistered(const QString &service);
    void serviceUnregistered(const QString &service);
    void serviceOwnerChanged(const QString &name, const QString &oldOwner, const QString &newOwner);

    void activate(const QModelIndex &item);

    void logError(const QString &msg);
    void anchorClicked(const QUrl &url);

private:
    void logMessage(const QString &msg);

    QDBusConnection c;
    QString currentService;
    QTreeView *tree;
    QAction *refreshAction;
    QTreeWidget *services;
    QTextBrowser *log;
};

#endif
