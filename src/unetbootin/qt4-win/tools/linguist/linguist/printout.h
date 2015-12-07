/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#ifndef PRINTOUT_H
#define PRINTOUT_H

#include <QFont>
#include <QPainter>
#include <QRect>
#include <QList>
#include <QDateTime>

QT_BEGIN_NAMESPACE

class QPrinter;
class QFontMetrics;

class PrintOut
{
public:
    enum Rule { NoRule, ThinRule, ThickRule };
    enum Style { Normal, Strong, Emphasis };

    PrintOut( QPrinter *printer );
    ~PrintOut();

    void setRule( Rule rule );
    void setGuide( const QString& guide );
    void vskip();
    void flushLine( bool mayBreak = false );
    void addBox( int percent, const QString& text = QString(),
                 Style style = Normal,
                 int halign = Qt::AlignLeft | Qt::TextWordWrap ); //NEW WordBread -> TextWordWrap

    int pageNum() const { return page; }

    struct Box
    {
        QRect rect;
        QString text;
        QFont font;
        int align;

        Box() : align( 0 ) { }
        Box( const QRect& r, const QString& t, const QFont& f, int a )
            : rect( r ), text( t ), font( f ), align( a ) { }
        Box( const Box& b )
            : rect( b.rect ), text( b.text ), font( b.font ),
              align( b.align ) { }

        Box& operator=( const Box& b ) {
            rect = b.rect;
            text = b.text;
            font = b.font;
            align = b.align;
            return *this;
        }

        bool operator==( const Box& b ) const {
            return rect == b.rect && text == b.text && font == b.font &&
                   align == b.align;
        }
    };

private:
    void breakPage(bool init = false);
    void drawRule( Rule rule );

    struct Paragraph {
        QRect rect;
        QList<Box> boxes;

        Paragraph() { }
        Paragraph( QPoint p ) : rect( p, QSize(0, 0) ) { }
    };

    QPrinter *pr;
    QPainter p;
    QFont f8;
    QFont f10;
    QFontMetrics *fmetrics;
    Rule nextRule;
    Paragraph cp;
    int page;
    bool firstParagraph;
    QString g;
    QDateTime dateTime;

    int hmargin;
    int vmargin;
    int voffset;
    int hsize;
    int vsize;
};

QT_END_NAMESPACE

#endif
