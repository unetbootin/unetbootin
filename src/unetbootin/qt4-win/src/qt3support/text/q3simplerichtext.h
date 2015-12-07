/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3SIMPLERICHTEXT_H
#define Q3SIMPLERICHTEXT_H

#include <QtCore/qnamespace.h>
#include <QtCore/qstring.h>
#include <QtGui/qregion.h>
#include <QtGui/qcolor.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

#ifndef QT_NO_RICHTEXT

class QPainter;
class QWidget;
class Q3StyleSheet;
class QBrush;
class Q3MimeSourceFactory;
class Q3SimpleRichTextData;

class Q_COMPAT_EXPORT Q3SimpleRichText
{
public:
    Q3SimpleRichText(const QString& text, const QFont& fnt,
                     const QString& context = QString(), const Q3StyleSheet* sheet = 0);
    Q3SimpleRichText(const QString& text, const QFont& fnt,
                     const QString& context, const Q3StyleSheet *sheet,
                     const Q3MimeSourceFactory* factory, int pageBreak = -1,
                     const QColor& linkColor = Qt::blue, bool linkUnderline = true);
    ~Q3SimpleRichText();

    void setWidth(int);
    void setWidth(QPainter*, int);
    void setDefaultFont(const QFont &f);
    int width() const;
    int widthUsed() const;
    int height() const;
    void adjustSize();

    void draw(QPainter* p, int x, int y, const QRect& clipRect,
               const QColorGroup& cg, const QBrush* paper = 0) const;

    void draw(QPainter* p, int x, int y, const QRegion& clipRegion,
               const QColorGroup& cg, const QBrush* paper = 0) const {
        draw(p, x, y, clipRegion.boundingRect(), cg, paper);
    }

    QString context() const;
    QString anchorAt(const QPoint& pos) const;

    bool inText(const QPoint& pos) const;

private:
    Q_DISABLE_COPY(Q3SimpleRichText)

    Q3SimpleRichTextData* d;
};

#endif // QT_NO_RICHTEXT

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3SIMPLERICHTEXT_H
