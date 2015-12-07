/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the documentation of the Qt Toolkit.
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

#include <QApplication>
#include <QBuffer>
#include <QPalette>

static void main_snippet()
{
//! [0]
    QBuffer buffer;
    char ch;

    buffer.open(QBuffer::ReadWrite);
    buffer.write("Qt rocks!");
    buffer.seek(0);
    buffer.getChar(&ch);  // ch == 'Q'
    buffer.getChar(&ch);  // ch == 't'
    buffer.getChar(&ch);  // ch == ' '
    buffer.getChar(&ch);  // ch == 'r'
//! [0]
}

static void write_datastream_snippets()
{
//! [1]
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);

    QDataStream out(&buffer);
    out << QApplication::palette();
//! [1]
}

static void read_datastream_snippets()
{
    QByteArray byteArray;

//! [2]
    QPalette palette;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::ReadOnly);

    QDataStream in(&buffer);
    in >> palette;
//! [2]
}

static void bytearray_ptr_ctor_snippet()
{
//! [3]
    QByteArray byteArray("abc");
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    buffer.seek(3);
    buffer.write("def", 3);
    buffer.close();
    // byteArray == "abcdef"
//! [3]
}

static void setBuffer_snippet()
{
//! [4]
    QByteArray byteArray("abc");
    QBuffer buffer;
    buffer.setBuffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    buffer.seek(3);
    buffer.write("def", 3);
    buffer.close();
    // byteArray == "abcdef"
//! [4]
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    main_snippet();
    bytearray_ptr_ctor_snippet();
    write_datastream_snippets();
    read_datastream_snippets();
    setBuffer_snippet();
    return 0;
}
