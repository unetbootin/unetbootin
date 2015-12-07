/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#ifndef QTCOLORLINE_H
#define QTCOLORLINE_H

#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class QtColorLine : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(int indicatorSpace READ indicatorSpace WRITE setIndicatorSpace)
    Q_PROPERTY(int indicatorSize READ indicatorSize WRITE setIndicatorSize)
    Q_PROPERTY(bool flip READ flip WRITE setFlip)
    Q_PROPERTY(bool backgroundCheckered READ isBackgroundCheckered WRITE setBackgroundCheckered)
    Q_PROPERTY(ColorComponent colorComponent READ colorComponent WRITE setColorComponent)
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
    Q_ENUMS(ColorComponent)
public:

    enum ColorComponent {
        Red,
        Green,
        Blue,
        Hue,
        Saturation,
        Value,
        Alpha
    };

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    QtColorLine(QWidget *parent = 0);
    ~QtColorLine();

    QColor color() const;

    void setIndicatorSize(int size);
    int indicatorSize() const;

    void setIndicatorSpace(int space);
    int indicatorSpace() const;

    void setFlip(bool flip);
    bool flip() const;

    bool isBackgroundCheckered() const;
    void setBackgroundCheckered(bool checkered);

    void setOrientation(Qt::Orientation orientation);
    Qt::Orientation orientation() const;

    void setColorComponent(ColorComponent component);
    ColorComponent colorComponent() const;

public slots:

    void setColor(const QColor &color);

signals:

    void colorChanged(const QColor &color);

protected:

    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

private:

    class QtColorLinePrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtColorLine)
    Q_DISABLE_COPY(QtColorLine)
};

QT_END_NAMESPACE

#endif
