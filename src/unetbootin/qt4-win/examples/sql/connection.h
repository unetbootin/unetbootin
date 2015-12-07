/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

/*
    This file defines a helper function to open a connection to an
    in-memory SQLITE database and to create a test table.

    If you want to use another database, simply modify the code
    below. All the examples in this directory use this function to
    connect to a database.
*/
//! [0]
static bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        QMessageBox::critical(0, qApp->tr("Cannot open database"),
            qApp->tr("Unable to establish a database connection.\n"
                     "This example needs SQLite support. Please read "
                     "the Qt SQL driver documentation for information how "
                     "to build it.\n\n"
                     "Click Cancel to exit."), QMessageBox::Cancel);
        return false;
    }

    QSqlQuery query;
    query.exec("create table person (id int primary key, "
               "firstname varchar(20), lastname varchar(20))");
    query.exec("insert into person values(101, 'Danny', 'Young')");
    query.exec("insert into person values(102, 'Christine', 'Holand')");
    query.exec("insert into person values(103, 'Lars', 'Gordon')");
    query.exec("insert into person values(104, 'Roberto', 'Robitaille')");
    query.exec("insert into person values(105, 'Maria', 'Papadopoulos')");

    query.exec("create table trolltechoffice (id int primary key,"
                                             "imagefile int,"
                                             "location varchar(20),"
                                             "country varchar(20),"
                                             "description varchar(100))");
    query.exec("insert into trolltechoffice "
               "values(0, 0, 'Oslo', 'Norway',"
               "'Oslo is home to more than 500 000 citizens and has a "
               "lot to offer.It has been called \"The city with the big "
               "heart\" and this is a nickname we are happy to live up to.')");
    query.exec("insert into trolltechoffice "
               "values(1, 1, 'Brisbane', 'Australia',"
               "'Brisbane is the capital of Queensland, the Sunshine State, "
               "where it is beautiful one day, perfect the next.  "
               "Brisbane is Australia''s 3rd largest city, being home "
               "to almost 2 million people.')");
    query.exec("insert into trolltechoffice "
               "values(2, 2, 'Redwood City', 'US',"
               "'You find Redwood City in the heart of the Bay Area "
               "just north of Silicon Valley. The largest nearby city is "
               "San Jose which is the third largest city in California "
               "and the 10th largest in the US.')");
    query.exec("insert into trolltechoffice "
               "values(3, 3, 'Berlin', 'Germany',"
               "'Berlin, the capital of Germany is dynamic, cosmopolitan "
               "and creative, allowing for every kind of lifestyle. "
               "East meets West in the metropolis at the heart of a "
               "changing Europe.')");
    query.exec("insert into trolltechoffice "
               "values(4, 4, 'Munich', 'Germany',"
               "'Several technology companies are represented in Munich, "
               "and the city is often called the \"Bavarian Silicon Valley\". "
               "The exciting city is also filled with culture, "
               "art and music. ')");
    query.exec("insert into trolltechoffice "
               "values(5, 5, 'Beijing', 'China',"
               "'Beijing as a capital city has more than 3000 years of "
               "history. Today the city counts 12 million citizens, and "
               "is the political, economic and cultural centre of China.')");

    query.exec("create table images (locationid int, file varchar(20))");
    query.exec("insert into images values(0, 'images/oslo.png')");
    query.exec("insert into images values(1, 'images/brisbane.png')");
    query.exec("insert into images values(2, 'images/redwood.png')");
    query.exec("insert into images values(3, 'images/berlin.png')");
    query.exec("insert into images values(4, 'images/munich.png')");
    query.exec("insert into images values(5, 'images/beijing.png')");



    return true;
}
//! [0]

#endif
