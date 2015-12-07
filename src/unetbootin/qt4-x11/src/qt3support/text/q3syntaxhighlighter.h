/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3SYNTAXHIGHLIGHTER_H
#define Q3SYNTAXHIGHLIGHTER_H

#include <QtGui/qfont.h>
#include <QtGui/qcolor.h>
#include <QtCore/qstring.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

class Q3TextEdit;
class Q3SyntaxHighlighterInternal;
class Q3SyntaxHighlighterPrivate;
class Q3TextParagraph;

class Q_COMPAT_EXPORT Q3SyntaxHighlighter
{
    friend class Q3SyntaxHighlighterInternal;

public:
    Q3SyntaxHighlighter(Q3TextEdit *textEdit);
    virtual ~Q3SyntaxHighlighter();

    virtual int highlightParagraph(const QString &text, int endStateOfLastPara) = 0;

    void setFormat(int start, int count, const QFont &font, const QColor &color);
    void setFormat(int start, int count, const QColor &color);
    void setFormat(int start, int count, const QFont &font);
    Q3TextEdit *textEdit() const { return edit; }

    void rehighlight();

    int currentParagraph() const;

private:
    Q3TextParagraph *para;
    Q3TextEdit *edit;
    Q3SyntaxHighlighterPrivate *d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3SYNTAXHIGHLIGHTER_H
