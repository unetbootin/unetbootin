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

#include "qpixeltool.h"

#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qpainter.h>
#include <qevent.h>
#include <qfiledialog.h>
#include <qsettings.h>
#include <qmenu.h>
#include <qactiongroup.h>

#include <qdebug.h>

QT_BEGIN_NAMESPACE

QPixelTool::QPixelTool(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(QLatin1String("PixelTool"));
    QSettings settings(QLatin1String("Trolltech"), QLatin1String("QPixelTool"));

    m_freeze = false;

    m_autoUpdate = settings.value(QLatin1String("autoUpdate"), 0).toBool();

    m_gridSize = settings.value(QLatin1String("gridSize"), 1).toInt();
    m_gridActive = settings.value(QLatin1String("gridActive"), 1).toInt();
    m_displayGridSize = false;
    m_displayGridSizeId = 0;

    m_zoom = settings.value(QLatin1String("zoom"), 4).toInt();

    m_displayZoom = false;
    m_displayZoomId = 0;

    m_preview_mode = false;

    m_currentColor = 0;

    m_mouseDown = false;

    m_initialSize = settings.value(QLatin1String("initialSize"), QSize(250, 200)).toSize();

    move(settings.value(QLatin1String("position")).toPoint());

    setMouseTracking(true);
    setAttribute(Qt::WA_NoBackground);
    m_updateId = startTimer(30);
}

QPixelTool::~QPixelTool()
{
    QSettings settings(QLatin1String("Trolltech"), QLatin1String("QPixelTool"));
    settings.setValue(QLatin1String("autoUpdate"), int(m_autoUpdate));
    settings.setValue(QLatin1String("gridSize"), m_gridSize);
    settings.setValue(QLatin1String("gridActive"), m_gridActive);
    settings.setValue(QLatin1String("zoom"), m_zoom);
    settings.setValue(QLatin1String("initialSize"), size());
    settings.setValue(QLatin1String("position"), pos());
}

void QPixelTool::setPreviewImage(const QImage &image)
{
    m_preview_mode = true;
    m_preview_image = image;
    m_freeze = true;
}

void QPixelTool::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_updateId && !m_freeze) {
        grabScreen();
    } else if (event->timerId() == m_displayZoomId) {
        killTimer(m_displayZoomId);
        setZoomVisible(false);
    } else if (event->timerId() == m_displayGridSizeId) {
        killTimer(m_displayGridSizeId);
        m_displayGridSize = false;
    }
}

void render_string(QPainter *p, int w, int h, const QString &text, int flags)
{
    p->setBrush(QColor(255, 255, 255, 191));
    p->setPen(Qt::black);
    QRect bounds;
    p->drawText(0, 0, w, h, Qt::TextDontPrint | flags, text, &bounds);

    if (bounds.x() == 0) bounds.adjust(0, 0, 10, 0);
    else bounds.adjust(-10, 0, 0, 0);

    if (bounds.y() == 0) bounds.adjust(0, 0, 0, 10);
    else bounds.adjust(0, -10, 0, 0);

    p->drawRect(bounds);
    p->drawText(bounds, flags, text);
}

void QPixelTool::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    if (m_preview_mode) {
        QPixmap pixmap(40, 40);
        QPainter pt(&pixmap);
        pt.fillRect(0, 0, 20, 20, Qt::white);
        pt.fillRect(20, 20, 20, 20, Qt::white);
        pt.fillRect(20, 0, 20, 20, Qt::lightGray);
        pt.fillRect(0, 20, 20, 20, Qt::lightGray);
        pt.end();
        p.fillRect(0, 0, width(), height(), pixmap);
    }

    int w = width();
    int h = height();

    p.save();
    p.scale(m_zoom, m_zoom);
    p.drawPixmap(0, 0, m_buffer);
    p.scale(1/m_zoom, 1/m_zoom);
    p.restore();

    // Draw the grid on top.
    if (m_gridActive) {
        p.setPen(m_gridActive == 1 ? Qt::black : Qt::white);
        int incr = m_gridSize * m_zoom;
        for (int x=0; x<w; x+=incr)
            p.drawLine(x, 0, x, h);
        for (int y=0; y<h; y+=incr)
            p.drawLine(0, y, w, y);
    }

    QFont f(QLatin1String("courier"));
    f.setBold(true);
    p.setFont(f);

    if (m_displayZoom) {
        render_string(&p, w, h,
                      QString(QLatin1String("Zoom: x%1")).arg(m_zoom),
                      Qt::AlignTop | Qt::AlignRight);
    }

    if (m_displayGridSize) {
        render_string(&p, w, h,
                      QString(QLatin1String("Grid size: %1")).arg(m_gridSize),
                      Qt::AlignBottom | Qt::AlignLeft);
    }

    if (m_freeze) {
        QString str;
        str.sprintf("%8X (%3d,%3d,%3d,%3d)",
                    m_currentColor,
                    (0xff000000 & m_currentColor) >> 24,
                    (0x00ff0000 & m_currentColor) >> 16,
                    (0x0000ff00 & m_currentColor) >> 8,
                    (0x000000ff & m_currentColor));
        render_string(&p, w, h,
                      str,
                      Qt::AlignBottom | Qt::AlignRight);
    }

    if (m_mouseDown && m_dragStart != m_dragCurrent) {
        int x1 = (m_dragStart.x() / m_zoom) * m_zoom;
        int y1 = (m_dragStart.y() / m_zoom) * m_zoom;
        int x2 = (m_dragCurrent.x() / m_zoom) * m_zoom;
        int y2 = (m_dragCurrent.y() / m_zoom) * m_zoom;
        QRect r = QRect(x1, y1, x2 - x1, y2 - y1).normalized();
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(Qt::red, 3, Qt::SolidLine));
        p.drawRect(r);
        p.setPen(QPen(Qt::black, 1, Qt::SolidLine));
        p.drawRect(r);

        QString str;
        str.sprintf("Rect: x=%d, y=%d, w=%d, h=%d",
                    r.x() / m_zoom,
                    r.y() / m_zoom,
                    r.width() / m_zoom,
                    r.height() / m_zoom);
        render_string(&p, w, h, str, Qt::AlignBottom | Qt::AlignLeft);
    }


}

void QPixelTool::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Space:
        toggleFreeze();
        break;
    case Qt::Key_Plus:
        setZoom(m_zoom + 1);
        break;
    case Qt::Key_Minus:
        setZoom(m_zoom - 1);
        break;
    case Qt::Key_PageUp:
        setGridSize(m_gridSize + 1);
        break;
    case Qt::Key_PageDown:
        setGridSize(m_gridSize - 1);
        break;
    case Qt::Key_G:
        toggleGrid();
        break;
    case Qt::Key_A:
        m_autoUpdate = !m_autoUpdate;
        break;
    case Qt::Key_C:
        if (e->modifiers() & Qt::ControlModifier)
            copyToClipboard();
        break;
    case Qt::Key_S:
        if (e->modifiers() & Qt::ControlModifier) {
            releaseKeyboard();
            saveToFile();
        }
        break;
    case Qt::Key_Control:
        grabKeyboard();
        break;
    }
}

void QPixelTool::keyReleaseEvent(QKeyEvent *e)
{
    switch(e->key()) {
    case Qt::Key_Control:
        releaseKeyboard();
        break;
    default:
        break;
    }
}

void QPixelTool::resizeEvent(QResizeEvent *)
{
    grabScreen();
}

void QPixelTool::mouseMoveEvent(QMouseEvent *e)
{
    if (m_mouseDown)
        m_dragCurrent = e->pos();

    int x = e->x() / m_zoom;
    int y = e->y() / m_zoom;

    QImage im = m_buffer.toImage().convertToFormat(QImage::Format_ARGB32);
    if (x < im.width() && y < im.height() && x >= 0 && y >= 0) {
        m_currentColor = im.pixel(x, y);
        update();
    }
}

void QPixelTool::mousePressEvent(QMouseEvent *e)
{
    if (!m_freeze)
        return;
    m_mouseDown = true;
    m_dragStart = e->pos();
}

void QPixelTool::mouseReleaseEvent(QMouseEvent *)
{
    m_mouseDown = false;
}

void QPixelTool::contextMenuEvent(QContextMenuEvent *e)
{
    bool tmpFreeze = m_freeze;
    m_freeze = true;

    QMenu menu;

    QAction title(QLatin1String("Qt Pixel Zooming Tool"), &menu);
    title.setEnabled(false);

    // Grid color options...
    QActionGroup gridGroup(this);
    QAction whiteGrid(QLatin1String("White grid"), &gridGroup);
    whiteGrid.setCheckable(true);
    whiteGrid.setChecked(m_gridActive == 2);
    whiteGrid.setShortcut(QKeySequence(Qt::Key_G));
    QAction blackGrid(QLatin1String("Black grid"), &gridGroup);
    blackGrid.setCheckable(true);
    blackGrid.setChecked(m_gridActive == 1);
    blackGrid.setShortcut(QKeySequence(Qt::Key_G));
    QAction noGrid(QLatin1String("No grid"), &gridGroup);
    noGrid.setCheckable(true);
    noGrid.setChecked(m_gridActive == 0);
    noGrid.setShortcut(QKeySequence(Qt::Key_G));

    // Grid size options
    QAction incrGrid(QLatin1String("Increase grid size"), &menu);
    incrGrid.setShortcut(QKeySequence(Qt::Key_PageUp));
    connect(&incrGrid, SIGNAL(triggered()), this, SLOT(increaseGridSize()));
    QAction decrGrid(QLatin1String("Decrease grid size"), &menu);
    decrGrid.setShortcut(QKeySequence(Qt::Key_PageDown));
    connect(&decrGrid, SIGNAL(triggered()), this, SLOT(decreaseGridSize()));

    // Zoom options
    QAction incrZoom(QLatin1String("Zoom in"), &menu);
    incrZoom.setShortcut(QKeySequence(Qt::Key_Plus));
    connect(&incrZoom, SIGNAL(triggered()), this, SLOT(increaseZoom()));
    QAction decrZoom(QLatin1String("Zoom out"), &menu);
    decrZoom.setShortcut(QKeySequence(Qt::Key_Minus));
    connect(&decrZoom, SIGNAL(triggered()), this, SLOT(decreaseZoom()));

    // Freeze / Autoupdate
    QAction freeze(QLatin1String("Frozen"), &menu);
    freeze.setCheckable(true);
    freeze.setChecked(tmpFreeze);
    freeze.setShortcut(QKeySequence(Qt::Key_Space));
    QAction autoUpdate(QLatin1String("Continous update"), &menu);
    autoUpdate.setCheckable(true);
    autoUpdate.setChecked(m_autoUpdate);
    autoUpdate.setShortcut(QKeySequence(Qt::Key_A));

    // Copy to clipboard / save
    QAction save(QLatin1String("Save as image"), &menu);
    save.setShortcut(QKeySequence(QLatin1String("Ctrl+S")));
    connect(&save, SIGNAL(triggered()), this, SLOT(saveToFile()));
    QAction copy(QLatin1String("Copy to clipboard"), &menu);
    copy.setShortcut(QKeySequence(QLatin1String("Ctrl+C")));
    connect(&copy, SIGNAL(triggered()), this, SLOT(copyToClipboard()));

    menu.addAction(&title);
    menu.addSeparator();
    menu.addAction(&whiteGrid);
    menu.addAction(&blackGrid);
    menu.addAction(&noGrid);
    menu.addSeparator();
    menu.addAction(&incrGrid);
    menu.addAction(&decrGrid);
    menu.addSeparator();
    menu.addAction(&incrZoom);
    menu.addAction(&decrZoom);
    menu.addSeparator();
    menu.addAction(&freeze);
    menu.addAction(&autoUpdate);
    menu.addSeparator();
    menu.addAction(&save);
    menu.addAction(&copy);

    menu.exec(mapToGlobal(e->pos()));

    // Read out grid settings
    if (noGrid.isChecked()) m_gridActive = 0;
    else if (blackGrid.isChecked()) m_gridActive = 1;
    else m_gridActive = 2;

    m_autoUpdate = autoUpdate.isChecked();
    tmpFreeze = freeze.isChecked();


    m_freeze = tmpFreeze;
}

QSize QPixelTool::sizeHint() const
{
    return m_initialSize;
}

void QPixelTool::grabScreen()
{
    if (m_preview_mode) {
        int w = qMin(width() / m_zoom + 1, m_preview_image.width());
        int h = qMin(height() / m_zoom + 1, m_preview_image.height());
        m_buffer = QPixmap::fromImage(m_preview_image).copy(0, 0, w, h);
        update();
        return;
    }

    QPoint mousePos = QCursor::pos();
    if (mousePos == m_lastMousePos && !m_autoUpdate)
        return;

    int w = int(width() / float(m_zoom));
    int h = int(height() / float(m_zoom));

    if (width() % m_zoom > 0)
        ++w;
    if (height() % m_zoom > 0)
        ++h;

    int x = mousePos.x() - w/2;
    int y = mousePos.y() - h/2;

    m_buffer = QPixmap::grabWindow(qApp->desktop()->winId(), x, y, w, h);

    QRegion geom(x, y, w, h);
    QRect screenRect;
    for (int i=0; i<qApp->desktop()->numScreens(); ++i)
        screenRect |= qApp->desktop()->screenGeometry(i);
    geom -= screenRect;
    QVector<QRect> rects = geom.rects();
    if (rects.size() > 0) {
        QPainter p(&m_buffer);
        p.translate(-x, -y);
        p.setPen(Qt::NoPen);
        p.setBrush(palette().color(QPalette::Dark));
        p.drawRects(rects);
    }

    update();

    m_lastMousePos = mousePos;
}

void QPixelTool::startZoomVisibleTimer()
{
    if (m_displayZoomId > 0) {
        killTimer(m_displayZoomId);
    }
    m_displayZoomId = startTimer(5000);
    setZoomVisible(true);
}

void QPixelTool::startGridSizeVisibleTimer()
{
    if (m_gridActive) {
        if (m_displayGridSizeId > 0)
            killTimer(m_displayGridSizeId);
        m_displayGridSizeId = startTimer(5000);
        m_displayGridSize = true;
        update();
    }
}

void QPixelTool::setZoomVisible(bool visible)
{
    m_displayZoom = visible;
    update();
}

void QPixelTool::toggleFreeze()
{
    m_freeze = !m_freeze;
    if (!m_freeze)
        m_dragStart = m_dragCurrent = QPoint();
}

void QPixelTool::setZoom(int zoom)
{
    if (zoom > 0) {
        QPoint pos = m_lastMousePos;
        m_lastMousePos = QPoint();
        m_zoom = zoom;
        grabScreen();
        m_lastMousePos = pos;
        m_dragStart = m_dragCurrent = QPoint();
        startZoomVisibleTimer();
    }
}

void QPixelTool::toggleGrid()
{
    if (++m_gridActive > 2)
        m_gridActive = 0;
    update();
}

void QPixelTool::setGridSize(int gridSize)
{
    if (m_gridActive && gridSize > 0) {
        m_gridSize = gridSize;
        startGridSizeVisibleTimer();
        update();
    }
}

void QPixelTool::copyToClipboard()
{
    QClipboard *cb = QApplication::clipboard();
    cb->setPixmap(m_buffer);
}

void QPixelTool::saveToFile()
{
    bool oldFreeze = m_freeze;
    m_freeze = true;
    QString name = QFileDialog::getSaveFileName(this, QLatin1String("Save as image"), QString(), QLatin1String("*.png"));
    if (!name.isEmpty()) {
        if (!name.endsWith(QLatin1String(".png")))
            name.append(QLatin1String(".png"));
        m_buffer.save(name, "PNG");
    }
    m_freeze = oldFreeze;
}

QT_END_NAMESPACE
