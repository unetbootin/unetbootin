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

#ifndef Q3GROUPBOX_H
#define Q3GROUPBOX_H

#include <QtGui/qgroupbox.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

class Q3GroupBoxPrivate;

class Q_COMPAT_EXPORT Q3GroupBox : public QGroupBox
{
    Q_OBJECT
public:
    enum
#if defined(Q_MOC_RUN)
    FrameShape
#else
    DummyFrame
#endif
    {   Box = QFrame::Box, Sunken = QFrame::Sunken, Plain = QFrame::Plain,
        Raised = QFrame::Raised, MShadow=QFrame::Shadow_Mask, NoFrame = QFrame::NoFrame,
        Panel = QFrame::Panel, StyledPanel = QFrame::StyledPanel, HLine = QFrame::HLine,
        VLine = QFrame::VLine,
        WinPanel = QFrame::WinPanel,ToolBarPanel = QFrame::StyledPanel,
        MenuBarPanel = QFrame::StyledPanel, PopupPanel = QFrame::StyledPanel,
        LineEditPanel = QFrame::StyledPanel,TabWidgetPanel = QFrame::StyledPanel,
        GroupBoxPanel = 0x0007,
        MShape = QFrame::Shape_Mask};

    typedef DummyFrame FrameShape;
    Q_ENUMS(FrameShape)

    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation DESIGNABLE false)
    Q_PROPERTY(int columns READ columns WRITE setColumns DESIGNABLE false)

    Q_PROPERTY(QRect frameRect READ frameRect WRITE setFrameRect DESIGNABLE false)
    Q_PROPERTY(FrameShape frameShape READ frameShape WRITE setFrameShape)
    Q_PROPERTY(FrameShape frameShadow READ frameShadow WRITE setFrameShadow)
    Q_PROPERTY(int lineWidth READ lineWidth WRITE setLineWidth)
    Q_PROPERTY(int midLineWidth READ midLineWidth WRITE setMidLineWidth)
    Q_PROPERTY(int margin READ margin WRITE setMargin)

public:
    explicit Q3GroupBox(QWidget* parent=0, const char* name=0);
    explicit Q3GroupBox(const QString &title,
	       QWidget* parent=0, const char* name=0);
    Q3GroupBox(int strips, Qt::Orientation o,
	       QWidget* parent=0, const char* name=0);
    Q3GroupBox(int strips, Qt::Orientation o, const QString &title,
	       QWidget* parent=0, const char* name=0);
    ~Q3GroupBox();

    virtual void setColumnLayout(int strips, Qt::Orientation o);

    int columns() const;
    void setColumns(int);

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation);

    int insideMargin() const;
    int insideSpacing() const;
    void setInsideMargin(int m);
    void setInsideSpacing(int s);

    void addSpace(int);

    void setFrameRect(QRect);
    QRect frameRect() const;
#ifdef qdoc
    void setFrameShadow(FrameShape);
    FrameShape frameShadow() const;
    void setFrameShape(FrameShape);
    FrameShape frameShape() const;
#else
    void setFrameShadow(DummyFrame);
    DummyFrame frameShadow() const;
    void setFrameShape(DummyFrame);
    DummyFrame frameShape() const;
#endif
    void setFrameStyle(int);
    int frameStyle() const;
    int frameWidth() const;
    void setLineWidth(int);
    int lineWidth() const;
    void setMargin(int margin) { setContentsMargins(margin, margin, margin, margin); }
    int margin() const
    { int margin; int dummy; getContentsMargins(&margin, &dummy, &dummy, &dummy);  return margin; }
    void setMidLineWidth(int);
    int midLineWidth() const;

protected:
    void childEvent(QChildEvent *);
    void resizeEvent(QResizeEvent *);
    void changeEvent(QEvent *);
    bool event(QEvent *);

private:
    void skip();
    void init();
    void calculateFrame();
    void insertWid(QWidget*);
    void drawFrame(QPainter *p);

    Q3GroupBoxPrivate * d;

    Q_DISABLE_COPY(Q3GroupBox)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3GROUPBOX_H
