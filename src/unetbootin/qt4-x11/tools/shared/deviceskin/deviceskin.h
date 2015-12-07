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

#ifndef SKIN_H
#define SKIN_H

#include <QtGui/QWidget>
#include <QtGui/QPolygon>
#include <QtGui/QRegion>
#include <QtGui/QPixmap>
#include <QtCore/QVector>

QT_BEGIN_NAMESPACE

namespace qvfb_internal {
    class CursorWindow;
}

class QTextStream;

// ------- Button Area
struct DeviceSkinButtonArea {
    DeviceSkinButtonArea();
    QString name;
    int	keyCode;
    QPolygon area;
    QString text;
    bool activeWhenClosed;
    bool toggleArea;
    bool toggleActiveArea;
};

// -------- Parameters
struct DeviceSkinParameters {
    enum ReadMode { ReadAll, ReadSizeOnly };
    bool read(const QString &skinDirectory,  ReadMode rm,  QString *errorMessage);
    bool read(QTextStream &ts, ReadMode rm, QString *errorMessage);

    QSize screenSize() const { return screenRect.size(); }
    QSize secondaryScreenSize() const;
    bool hasSecondaryScreen() const;

    QString skinImageUpFileName;
    QString skinImageDownFileName;
    QString skinImageClosedFileName;
    QString skinCursorFileName;

    QImage skinImageUp;
    QImage skinImageDown;
    QImage skinImageClosed;
    QImage skinCursor;

    QRect screenRect;
    QRect backScreenRect;
    QRect closedScreenRect;
    int screenDepth;
    QPoint cursorHot;
    QVector<DeviceSkinButtonArea> buttonAreas;
    QList<int> toggleAreaList;

    int joystick;
    QString prefix;
    bool hasMouseHover;
};

// --------- Skin Widget
class DeviceSkin : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceSkin(const DeviceSkinParameters &parameters,  QWidget *p );
    ~DeviceSkin( );

    QWidget *view() const { return m_view; }
    void setView( QWidget *v );

    QWidget *secondaryView() const { return m_secondaryView; }
    void setSecondaryView( QWidget *v );

    void setZoom( double );
    void setTransform( const QMatrix& );

    bool hasCursor() const;

    QString prefix() const  {return m_parameters.prefix;}

signals:
    void popupMenu();
    void skinKeyPressEvent(int code, const QString& text, bool autorep);
    void skinKeyReleaseEvent(int code, const QString& text, bool autorep);

protected slots:
    void skinKeyRepeat();
    void moveParent();

protected:
    virtual void paintEvent( QPaintEvent * );
    virtual void mousePressEvent( QMouseEvent *e );
    virtual void mouseMoveEvent( QMouseEvent *e );
    virtual void mouseReleaseEvent( QMouseEvent * );

private:
    void calcRegions();
    void flip(bool open);
    void updateSecondaryScreen();
    void loadImages();
    void startPress(int);
    void endPress();

    const DeviceSkinParameters m_parameters;
    QVector<QRegion> buttonRegions;
    QPixmap skinImageUp;
    QPixmap skinImageDown;
    QPixmap skinImageClosed;
    QPixmap skinCursor;
    QWidget *parent;
    QWidget  *m_view;
    QWidget *m_secondaryView;
    QPoint parentpos;
    QPoint clickPos;
    bool buttonPressed;
    int buttonIndex;
    QMatrix transform;
    qvfb_internal::CursorWindow *cursorw;

    bool joydown;
    QTimer *t_skinkey;
    QTimer *t_parentmove;
    int onjoyrelease;

    bool flipped_open;
};

QT_END_NAMESPACE

#endif
