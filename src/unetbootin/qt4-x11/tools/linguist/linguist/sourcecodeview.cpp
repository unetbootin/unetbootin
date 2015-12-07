/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "sourcecodeview.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

#include <QtGui/QTextCharFormat>
#include <QtGui/QTextBlock>
#include <QtGui/QTextCursor>

QT_BEGIN_NAMESPACE

SourceCodeView::SourceCodeView(QWidget *parent)
  : QPlainTextEdit(parent),
    m_isActive(true),
    m_lineNumToLoad(0)
{
    setReadOnly(true);
}

void SourceCodeView::setSourceContext(const QString &fileName, const int lineNum)
{
    m_fileToLoad.clear();
    setToolTip(fileName);

    if (fileName.isNull()) {
        clear();
        m_currentFileName.clear();
        appendHtml(tr("<i>Source code not available</i>"));
        return;
    }

    if (m_isActive) {
        showSourceCode(fileName, lineNum);
    } else {
        m_fileToLoad = fileName;
        m_lineNumToLoad = lineNum;
    }
}

void SourceCodeView::setActivated(bool activated)
{
    m_isActive = activated;
    if (activated && !m_fileToLoad.isEmpty()) {
        showSourceCode(m_fileToLoad, m_lineNumToLoad);
        m_fileToLoad.clear();
    }
}

void SourceCodeView::showSourceCode(const QString &absFileName, const int lineNum)
{
    QString fileText = fileHash.value(absFileName);

    if (fileText.isNull()) { // File not in hash
        m_currentFileName.clear();

        // Assume fileName is relative to directory
        QFile file(absFileName);

        if (!file.exists()) {
            clear();
            appendHtml(tr("<i>File %1 not available</i>").arg(absFileName));
            return;
        }
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            clear();
            appendHtml(tr("<i>File %1 not readable</i>").arg(absFileName));
            return;
        }
        fileText = QString::fromLatin1(file.readAll());
        fileHash.insert(absFileName, fileText);
    }


    if (m_currentFileName != absFileName) {
        setPlainText(fileText);
        m_currentFileName = absFileName;
    }

    QTextCursor cursor = textCursor();
    cursor.setPosition(document()->findBlockByNumber(lineNum - 1).position());
    setTextCursor(cursor);
    centerCursor();
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);

    QTextEdit::ExtraSelection selectedLine;
    selectedLine.cursor = cursor;

    // Define custom color for line selection
    const QColor fg = palette().color(QPalette::Highlight);
    const QColor bg = palette().color(QPalette::Base);
    QColor col;
    const qreal ratio = 0.25;
    col.setRedF(fg.redF() * ratio + bg.redF() * (1 - ratio));
    col.setGreenF(fg.greenF() * ratio + bg.greenF() * (1 - ratio));
    col.setBlueF(fg.blueF() * ratio + bg.blueF() * (1 - ratio));

    selectedLine.format.setBackground(col);
    selectedLine.format.setProperty(QTextFormat::FullWidthSelection, true);
    setExtraSelections(QList<QTextEdit::ExtraSelection>() << selectedLine);
}

QT_END_NAMESPACE
