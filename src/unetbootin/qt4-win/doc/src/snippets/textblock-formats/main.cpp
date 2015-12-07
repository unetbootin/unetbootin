/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
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

#include <QtGui>

QString tr(const char *text)
{
    return QApplication::translate(text, text);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

//! [0]
    QTextEdit *editor = new QTextEdit();
    QTextCursor cursor(editor->textCursor());
//! [0]
    cursor.movePosition(QTextCursor::Start); 

    QTextBlockFormat blockFormat = cursor.blockFormat();
    blockFormat.setTopMargin(4);
    blockFormat.setLeftMargin(4);
    blockFormat.setRightMargin(4);
    blockFormat.setBottomMargin(4);

    cursor.setBlockFormat(blockFormat);
    cursor.insertText(tr("This contains plain text inside a "
                         "text block with margins to keep it separate "
                         "from other parts of the document."));

    cursor.insertBlock();

//! [1]
    QTextBlockFormat backgroundFormat = blockFormat;
    backgroundFormat.setBackground(QColor("lightGray"));

    cursor.setBlockFormat(backgroundFormat);
//! [1]
    cursor.insertText(tr("The background color of a text block can be "
                         "changed to highlight text."));

    cursor.insertBlock();

    QTextBlockFormat rightAlignedFormat = blockFormat;
    rightAlignedFormat.setAlignment(Qt::AlignRight);

    cursor.setBlockFormat(rightAlignedFormat);
    cursor.insertText(tr("The alignment of the text within a block is "
                         "controlled by the alignment properties of "
                         "the block itself. This text block is "
                         "right-aligned."));

    cursor.insertBlock();

    QTextBlockFormat paragraphFormat = blockFormat;
    paragraphFormat.setAlignment(Qt::AlignJustify);
    paragraphFormat.setTextIndent(32);

    cursor.setBlockFormat(paragraphFormat);
    cursor.insertText(tr("Text can be formatted so that the first "
                         "line in a paragraph has its own margin. "
                         "This makes the text more readable."));

    cursor.insertBlock();

    QTextBlockFormat reverseFormat = blockFormat;
    reverseFormat.setAlignment(Qt::AlignJustify);
    reverseFormat.setTextIndent(32);

    cursor.setBlockFormat(reverseFormat);
    cursor.insertText(tr("The direction of the text can be reversed. "
                         "This is useful for right-to-left "
                         "languages."));

    editor->setWindowTitle(tr("Text Block Formats"));
    editor->resize(480, 480);
    editor->show();
    
    return app.exec();
}
