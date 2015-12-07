/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#include "stardelegate.h"
#include "stareditor.h"
#include "starrating.h"

//! [0]
void populateTableWidget(QTableWidget *tableWidget)
{
    static const struct {
        const char *title;
        const char *genre;
        const char *artist;
        int rating;
    } staticData[] = {
//! [0] //! [1]
        { "Mass in B-Minor", "Baroque", "J.S. Bach", 5 },
//! [1]
        { "Three More Foxes", "Jazz", "Maynard Ferguson", 4 },
        { "Sex Bomb", "Pop", "Tom Jones", 3 },
        { "Barbie Girl", "Pop", "Aqua", 5 },
//! [2]
        { 0, 0, 0, 0 }
//! [2] //! [3]
    };
//! [3] //! [4]

    for (int row = 0; staticData[row].title != 0; ++row) {
        QTableWidgetItem *item0 = new QTableWidgetItem(staticData[row].title);
        QTableWidgetItem *item1 = new QTableWidgetItem(staticData[row].genre);
        QTableWidgetItem *item2 = new QTableWidgetItem(staticData[row].artist);
        QTableWidgetItem *item3 = new QTableWidgetItem;
        item3->setData(0,
                       qVariantFromValue(StarRating(staticData[row].rating)));

        tableWidget->setItem(row, 0, item0);
        tableWidget->setItem(row, 1, item1);
        tableWidget->setItem(row, 2, item2);
        tableWidget->setItem(row, 3, item3);
    }
}
//! [4]

//! [5]
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTableWidget tableWidget(4, 4);
    tableWidget.setItemDelegate(new StarDelegate);
    tableWidget.setEditTriggers(QAbstractItemView::DoubleClicked
                                | QAbstractItemView::SelectedClicked);
    tableWidget.setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList headerLabels;
    headerLabels << "Title" << "Genre" << "Artist" << "Rating";
    tableWidget.setHorizontalHeaderLabels(headerLabels);

    populateTableWidget(&tableWidget);

    tableWidget.resizeColumnsToContents();
    tableWidget.resize(500, 300);
    tableWidget.show();

    return app.exec();
}
//! [5]
