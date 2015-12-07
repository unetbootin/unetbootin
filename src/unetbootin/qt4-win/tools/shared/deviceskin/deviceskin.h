/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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
    double zoom;
    qvfb_internal::CursorWindow *cursorw;

    bool joydown;
    QTimer *t_skinkey;
    QTimer *t_parentmove;
    int onjoyrelease;

    bool flipped_open;
};

QT_END_NAMESPACE

#endif
