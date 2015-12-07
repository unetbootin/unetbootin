/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QWidget>

QT_FORWARD_DECLARE_CLASS(QUndoStack)
QT_FORWARD_DECLARE_CLASS(QTextStream)

class Shape
{
public:
    enum Type { Rectangle, Circle, Triangle };

    Shape(Type type = Rectangle, const QColor &color = Qt::red, const QRect &rect = QRect());

    Type type() const;
    QString name() const;
    QRect rect() const;
    QRect resizeHandle() const;
    QColor color() const;

    static QString typeToString(Type type);
    static Type stringToType(const QString &s, bool *ok = 0);

    static const QSize minSize;

private:
    Type m_type;
    QRect m_rect;
    QColor m_color;
    QString m_name;

    friend class Document;
};

class Document : public QWidget
{
    Q_OBJECT

public:
    Document(QWidget *parent = 0);

    QString addShape(const Shape &shape);
    void deleteShape(const QString &shapeName);
    Shape shape(const QString &shapeName) const;
    QString currentShapeName() const;

    void setShapeRect(const QString &shapeName, const QRect &rect);
    void setShapeColor(const QString &shapeName, const QColor &color);

    bool load(QTextStream &stream);
    void save(QTextStream &stream);

    QString fileName() const;
    void setFileName(const QString &fileName);

    QUndoStack *undoStack() const;

signals:
    void currentShapeChanged(const QString &shapeName);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    void setCurrentShape(int index);
    int indexOf(const QString &shapeName) const;
    int indexAt(const QPoint &pos) const;
    QString uniqueName(const QString &name) const;

    QList<Shape> m_shapeList;
    int m_currentIndex;
    int m_mousePressIndex;
    QPoint m_mousePressOffset;
    bool m_resizeHandlePressed;
    QString m_fileName;

    QUndoStack *m_undoStack;
};

#endif // DOCUMENT_H
