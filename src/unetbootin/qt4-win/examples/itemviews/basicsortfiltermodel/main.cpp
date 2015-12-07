/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

#include <QtGui>

#include "window.h"

void addMail(QAbstractItemModel *model, const QString &subject,
             const QString &sender, const QDateTime &date)
{
    model->insertRow(0);
    model->setData(model->index(0, 0), subject);
    model->setData(model->index(0, 1), sender);
    model->setData(model->index(0, 2), date);
}

QAbstractItemModel *createMailModel(QObject *parent)
{
    QStandardItemModel *model = new QStandardItemModel(0, 3, parent);

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Subject"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Sender"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Date"));

    addMail(model, "Happy New Year!", "Grace K. <grace@software-inc.com>",
            QDateTime(QDate(2006, 12, 31), QTime(17, 03)));
    addMail(model, "Radically new concept", "Grace K. <grace@software-inc.com>",
            QDateTime(QDate(2006, 12, 22), QTime(9, 44)));
    addMail(model, "Accounts", "pascale@nospam.com",
            QDateTime(QDate(2006, 12, 31), QTime(12, 50)));
    addMail(model, "Expenses", "Joe Bloggs <joe@bloggs.com>",
            QDateTime(QDate(2006, 12, 25), QTime(11, 39)));
    addMail(model, "Re: Expenses", "Andy <andy@nospam.com>",
            QDateTime(QDate(2007, 01, 02), QTime(16, 05)));
    addMail(model, "Re: Accounts", "Joe Bloggs <joe@bloggs.com>",
            QDateTime(QDate(2007, 01, 03), QTime(14, 18)));
    addMail(model, "Re: Accounts", "Andy <andy@nospam.com>",
            QDateTime(QDate(2007, 01, 03), QTime(14, 26)));
    addMail(model, "Sports", "Linda Smith <linda.smith@nospam.com>",
            QDateTime(QDate(2007, 01, 05), QTime(11, 33)));
    addMail(model, "AW: Sports", "Rolf Newschweinstein <rolfn@nospam.com>",
            QDateTime(QDate(2007, 01, 05), QTime(12, 00)));
    addMail(model, "RE: Sports", "Petra Schmidt <petras@nospam.com>",
            QDateTime(QDate(2007, 01, 05), QTime(12, 01)));

    return model;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Window window;
    window.setSourceModel(createMailModel(&window));
    window.show();
    return app.exec();
}
