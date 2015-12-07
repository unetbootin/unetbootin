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

#ifndef QTEXTDOCUMENTFRAGMENT_P_H
#define QTEXTDOCUMENTFRAGMENT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtGui/qtextdocument.h"
#include "private/qtexthtmlparser_p.h"
#include "private/qtextdocument_p.h"
#include "QtGui/qtexttable.h"
#include "QtCore/qatomic.h"
#include "QtCore/qlist.h"
#include "QtCore/qmap.h"
#include "QtCore/qpointer.h"
#include "QtCore/qvarlengtharray.h"
#include "QtCore/qdatastream.h"

QT_BEGIN_NAMESPACE

class QTextDocumentFragmentPrivate;

class QTextCopyHelper
{
public:
    QTextCopyHelper(const QTextCursor &_source, const QTextCursor &_destination, bool forceCharFormat = false, const QTextCharFormat &fmt = QTextCharFormat());

    void copy();

private:
    void appendFragments(int pos, int endPos);
    int appendFragment(int pos, int endPos, int objectIndex = -1);
    int convertFormatIndex(const QTextFormat &oldFormat, int objectIndexToSet = -1);
    inline int convertFormatIndex(int oldFormatIndex, int objectIndexToSet = -1)
    { return convertFormatIndex(src->formatCollection()->format(oldFormatIndex), objectIndexToSet); }
    inline QTextFormat convertFormat(const QTextFormat &fmt)
    { return dst->formatCollection()->format(convertFormatIndex(fmt)); }

    int insertPos;

    bool forceCharFormat;
    int primaryCharFormatIndex;

    QTextCursor cursor;
    QTextDocumentPrivate *dst;
    QTextDocumentPrivate *src;
    QTextFormatCollection &formatCollection;
    const QString originalText;
    QMap<int, int> objectIndexMap;
};

class QTextDocumentFragmentPrivate
{
public:
    QTextDocumentFragmentPrivate(const QTextCursor &cursor = QTextCursor());
    inline ~QTextDocumentFragmentPrivate() { delete doc; }

    void insert(QTextCursor &cursor) const;

    QAtomicInt ref;
    QTextDocument *doc;

    uint importedFromPlainText : 1;
private:
    Q_DISABLE_COPY(QTextDocumentFragmentPrivate)
};

#ifndef QT_NO_TEXTHTMLPARSER

class QTextHtmlImporter : public QTextHtmlParser
{
    struct Table;
public:
    enum ImportMode {
        ImportToFragment,
        ImportToDocument
    };

    QTextHtmlImporter(QTextDocument *_doc, const QString &html,
                      ImportMode mode,
                      const QTextDocument *resourceProvider = 0);

    void import();

private:
    bool closeTag();

    Table scanTable(int tableNodeIdx);

    enum ProcessNodeResult { ContinueWithNextNode, ContinueWithCurrentNode };

    void appendBlock(const QTextBlockFormat &format, QTextCharFormat charFmt = QTextCharFormat());
    bool appendNodeText();

    ProcessNodeResult processBlockNode();
    ProcessNodeResult processSpecialNodes();

    struct List
    {
        inline List() : listNode(0) {}
        QTextListFormat format;
        int listNode;
        QPointer<QTextList> list;
    };
    QVector<List> lists;
    int indent;

    // insert a named anchor the next time we emit a char format,
    // either in a block or in regular text
    QStringList namedAnchors;

#ifdef Q_CC_SUN
    friend struct QTextHtmlImporter::Table;
#endif
    struct TableCellIterator
    {
        inline TableCellIterator(QTextTable *t = 0) : table(t), row(0), column(0) {}

        inline TableCellIterator &operator++() {
            if (atEnd())
                return *this;
            do {
                const QTextTableCell cell = table->cellAt(row, column);
                if (!cell.isValid())
                    break;
                column += cell.columnSpan();
                if (column >= table->columns()) {
                    column = 0;
                    ++row;
                }
            } while (row < table->rows() && table->cellAt(row, column).row() != row);

            return *this;
        }

        inline bool atEnd() const { return table == 0 || row >= table->rows(); }

        QTextTableCell cell() const { return table->cellAt(row, column); }

        QTextTable *table;
        int row;
        int column;
    };

    friend struct Table;
    struct Table
    {
        Table() : isTextFrame(false), rows(0), columns(0), currentRow(0), lastIndent(0) {}
        QPointer<QTextFrame> frame;
        bool isTextFrame;
        int rows;
        int columns;
        int currentRow; // ... for buggy html (see html_skipCell testcase)
        TableCellIterator currentCell;
        int lastIndent;
    };
    QVector<Table> tables;

    struct RowColSpanInfo
    {
        int row, col;
        int rowSpan, colSpan;
    };

    enum WhiteSpace
    {
        RemoveWhiteSpace,
        CollapseWhiteSpace,
        PreserveWhiteSpace
    };

    WhiteSpace compressNextWhitespace;

    QTextDocument *doc;
    QTextCursor cursor;
    QTextHtmlParserNode::WhiteSpaceMode wsm;
    ImportMode importMode;
    bool hasBlock;
    bool forceBlockMerging;
    bool blockTagClosed;
    int currentNodeIdx;
    const QTextHtmlParserNode *currentNode;
};

QT_END_NAMESPACE
#endif // QT_NO_TEXTHTMLPARSER

#endif // QTEXTDOCUMENTFRAGMENT_P_H
