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

#ifndef QFRAME_H
#define QFRAME_H

#include <QtGui/qwidget.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QFramePrivate;

class Q_GUI_EXPORT QFrame : public QWidget
{
    Q_OBJECT

    Q_ENUMS(Shape Shadow)
    Q_PROPERTY(Shape frameShape READ frameShape WRITE setFrameShape)
    Q_PROPERTY(Shadow frameShadow READ frameShadow WRITE setFrameShadow)
    Q_PROPERTY(int lineWidth READ lineWidth WRITE setLineWidth)
    Q_PROPERTY(int midLineWidth READ midLineWidth WRITE setMidLineWidth)
    Q_PROPERTY(int frameWidth READ frameWidth)
    Q_PROPERTY(QRect frameRect READ frameRect WRITE setFrameRect DESIGNABLE false)

public:
    explicit QFrame(QWidget* parent = 0, Qt::WindowFlags f = 0);
    ~QFrame();

    int frameStyle() const;
    void setFrameStyle(int);

    int frameWidth() const;

    QSize sizeHint() const;

    enum Shape {
        NoFrame  = 0, // no frame
        Box = 0x0001, // rectangular box
        Panel = 0x0002, // rectangular panel
        WinPanel = 0x0003, // rectangular panel (Windows)
        HLine = 0x0004, // horizontal line
        VLine = 0x0005, // vertical line
        StyledPanel = 0x0006 // rectangular panel depending on the GUI style

#if defined(QT3_SUPPORT) && !defined(Q_MOC_RUN)
        ,PopupPanel = StyledPanel, // rectangular panel depending on the GUI style
        MenuBarPanel = StyledPanel,
        ToolBarPanel = StyledPanel,
        LineEditPanel = StyledPanel,
        TabWidgetPanel = StyledPanel,
        GroupBoxPanel = StyledPanel
#endif
    };
    enum Shadow {
        Plain = 0x0010, // plain line
        Raised = 0x0020, // raised shadow effect
        Sunken = 0x0030 // sunken shadow effect
    };

    enum StyleMask {
        Shadow_Mask = 0x00f0, // mask for the shadow
        Shape_Mask = 0x000f // mask for the shape
#if defined(QT3_SUPPORT)
        ,MShadow = Shadow_Mask,
        MShape = Shape_Mask
#endif
    };

    Shape frameShape() const;
    void setFrameShape(Shape);
    Shadow frameShadow() const;
    void setFrameShadow(Shadow);

    int lineWidth() const;
    void setLineWidth(int);

    int midLineWidth() const;
    void setMidLineWidth(int);

    QRect frameRect() const;
    void setFrameRect(const QRect &);

protected:
    bool event(QEvent *e);
    void paintEvent(QPaintEvent *);
    void changeEvent(QEvent *);
    void drawFrame(QPainter *);

#ifdef QT3_SUPPORT
public:
    QT3_SUPPORT_CONSTRUCTOR QFrame(QWidget* parent, const char* name, Qt::WindowFlags f = 0);
#endif

protected:
    QFrame(QFramePrivate &dd, QWidget* parent = 0, Qt::WindowFlags f = 0);

private:
    Q_DISABLE_COPY(QFrame)
    Q_DECLARE_PRIVATE(QFrame)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QFRAME_H
