/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QTEXTTABLE_H
#define QTEXTTABLE_H

#include <QtCore/qglobal.h>
#include <QtCore/qobject.h>
#include <QtGui/qtextobject.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QTextCursor;
class QTextTable;
class QTextTablePrivate;

class Q_GUI_EXPORT QTextTableCell
{
public:
    QTextTableCell() : table(0) {}
    ~QTextTableCell() {}
    QTextTableCell(const QTextTableCell &o) : table(o.table), fragment(o.fragment) {}
    QTextTableCell &operator=(const QTextTableCell &o)
    { table = o.table; fragment = o.fragment; return *this; }

    void setFormat(const QTextCharFormat &format);
    QTextCharFormat format() const;

    int row() const;
    int column() const;

    int rowSpan() const;
    int columnSpan() const;

    inline bool isValid() const { return table != 0; }

    QTextCursor firstCursorPosition() const;
    QTextCursor lastCursorPosition() const;
    int firstPosition() const;
    int lastPosition() const;

    inline bool operator==(const QTextTableCell &other) const
    { return table == other.table && fragment == other.fragment; }
    inline bool operator!=(const QTextTableCell &other) const
    { return !operator==(other); }

    QTextFrame::iterator begin() const;
    QTextFrame::iterator end() const;

private:
    friend class QTextTable;
    QTextTableCell(const QTextTable *t, int f)
        : table(t), fragment(f) {}

    const QTextTable *table;
    int fragment;
};

class Q_GUI_EXPORT QTextTable : public QTextFrame
{
    Q_OBJECT
public:
    explicit QTextTable(QTextDocument *doc);
    ~QTextTable();

    void resize(int rows, int cols);
    void insertRows(int pos, int num);
    void insertColumns(int pos, int num);
    void removeRows(int pos, int num);
    void removeColumns(int pos, int num);

    void mergeCells(int row, int col, int numRows, int numCols);
    void mergeCells(const QTextCursor &cursor);
    void splitCell(int row, int col, int numRows, int numCols);

    int rows() const;
    int columns() const;

    QTextTableCell cellAt(int row, int col) const;
    QTextTableCell cellAt(int position) const;
    QTextTableCell cellAt(const QTextCursor &c) const;

    QTextCursor rowStart(const QTextCursor &c) const;
    QTextCursor rowEnd(const QTextCursor &c) const;

    void setFormat(const QTextTableFormat &format);
    QTextTableFormat format() const { return QTextObject::format().toTableFormat(); }

private:
    Q_DISABLE_COPY(QTextTable)
    Q_DECLARE_PRIVATE(QTextTable)
    friend class QTextTableCell;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QTEXTTABLE_H
