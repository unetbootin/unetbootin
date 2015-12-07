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

#ifndef QCLEANLOOKSSTYLE_H
#define QCLEANLOOKSSTYLE_H

#include <QtGui/qwindowsstyle.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#if !defined(QT_NO_STYLE_CLEANLOOKS)

class QCleanlooksStylePrivate;
class Q_GUI_EXPORT QCleanlooksStyle : public QWindowsStyle
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QCleanlooksStyle)

public:
    QCleanlooksStyle();
    ~QCleanlooksStyle();

    QPalette standardPalette () const;
    void drawPrimitive(PrimitiveElement elem,
                        const QStyleOption *option,
                        QPainter *painter, const QWidget *widget = 0) const;
    void drawControl(ControlElement ce, const QStyleOption *option, QPainter *painter,
                                const QWidget *widget) const;
    int pixelMetric(PixelMetric metric, const QStyleOption *option = 0, const QWidget *widget = 0) const;
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                            QPainter *painter, const QWidget *widget) const;
    QRect subElementRect(SubElement r, const QStyleOption *opt, const QWidget *widget = 0) const;
    QSize sizeFromContents(ContentsType type, const QStyleOption *option,
                           const QSize &size, const QWidget *widget) const;
    SubControl hitTestComplexControl(ComplexControl cc, const QStyleOptionComplex *opt,
                               const QPoint &pt, const QWidget *w = 0) const;
    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt,
                         SubControl sc, const QWidget *widget) const;
    QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
                                        const QStyleOption *opt) const;
    int styleHint(StyleHint hint, const QStyleOption *option = 0, const QWidget *widget = 0,
                  QStyleHintReturn *returnData = 0) const;
    QRect itemPixmapRect(const QRect &r, int flags, const QPixmap &pixmap) const;
    QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt,
                           const QWidget *widget = 0) const;
    void drawItemPixmap(QPainter *painter, const QRect &rect,
                        int alignment, const QPixmap &pixmap) const;
    void drawItemText(QPainter *painter, const QRect &rect,
                              int flags, const QPalette &pal, bool enabled,
                              const QString &text, QPalette::ColorRole textRole = QPalette::NoRole) const;
    void polish(QWidget *widget);
    void polish(QApplication *app);
    void polish(QPalette &pal);
    void unpolish(QWidget *widget);
    void unpolish(QApplication *app);

protected Q_SLOTS:
    QIcon standardIconImplementation(StandardPixmap standardIcon, const QStyleOption *option,
                                     const QWidget *widget = 0) const;

};

#endif // QT_NO_STYLE_CLEANLOOKS

QT_END_NAMESPACE

QT_END_HEADER

#endif // QCLEANLOOKSSTYLE_H
