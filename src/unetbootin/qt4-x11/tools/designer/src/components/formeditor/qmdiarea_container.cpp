/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#include "qmdiarea_container.h"

#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerFormEditorInterface>

#include <QtGui/QMdiArea>
#include <QtGui/QMdiSubWindow>
#include <QtGui/QApplication>
#include <QtCore/QDebug>
#include <QtCore/QHash>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

QMdiAreaContainer::QMdiAreaContainer(QMdiArea *widget, QObject *parent)
    : QObject(parent),
      m_mdiArea(widget)
{
}

int QMdiAreaContainer::count() const
{
    return m_mdiArea->subWindowList(QMdiArea::CreationOrder).count();
}

QWidget *QMdiAreaContainer::widget(int index) const
{
    if (index < 0)
        return 0;
    return m_mdiArea->subWindowList(QMdiArea::CreationOrder).at(index)->widget();
}

int QMdiAreaContainer::currentIndex() const
{
    if (QMdiSubWindow *sub = m_mdiArea->activeSubWindow())
        return m_mdiArea->subWindowList(QMdiArea::CreationOrder).indexOf(sub);
    return -1;
}

void QMdiAreaContainer::setCurrentIndex(int index)
{
    if (index < 0) {
        qDebug() << "** WARNING Attempt to QMdiAreaContainer::setCurrentIndex(-1)";
        return;
    }
    QMdiSubWindow *frame = m_mdiArea->subWindowList(QMdiArea::CreationOrder).at(index);
    m_mdiArea->setActiveSubWindow(frame);
}

void QMdiAreaContainer::addWidget(QWidget *widget)
{
    QMdiSubWindow *frame = m_mdiArea->addSubWindow(widget, Qt::Window);
    frame->show();
    m_mdiArea->cascadeSubWindows();
    positionNewMdiChild(m_mdiArea, frame);
}

// Semi-smart positioning of new windows: Make child fill the whole MDI window below
// cascaded other windows
void QMdiAreaContainer::positionNewMdiChild(const QWidget *area, QWidget *mdiChild)
{
    enum { MinSize = 20 };
    const QPoint pos = mdiChild->pos();
    const QSize areaSize = area->size();
    switch (QApplication::layoutDirection()) {
    case Qt::LeftToRight: {
        const QSize fullSize = QSize(areaSize.width() - pos.x(), areaSize.height() - pos.y());
        if (fullSize.width() > MinSize && fullSize.height() > MinSize)
            mdiChild->resize(fullSize);
    }
        break;
    case Qt::RightToLeft: {
        const QSize fullSize = QSize(pos.x() + mdiChild->width(), areaSize.height() - pos.y());
        if (fullSize.width() > MinSize && fullSize.height() > MinSize) {
            mdiChild->move(0, pos.y());
            mdiChild->resize(fullSize);
        }
    }
        break;
    }
}

void QMdiAreaContainer::insertWidget(int, QWidget *widget)
{
    addWidget(widget);
}

void QMdiAreaContainer::remove(int index)
{
    QList<QMdiSubWindow *> subWins = m_mdiArea->subWindowList(QMdiArea::CreationOrder);
    if (index >= 0 && index < subWins.size()) {
        QMdiSubWindow *f = subWins.at(index);
        m_mdiArea->removeSubWindow(f->widget());
        delete f;
    }
}

// ---------- MdiAreaPropertySheet, creates fake properties:
// 1) window name (object name of child)
// 2) title (windowTitle of child).

static const char *subWindowTitleC = "activeSubWindowTitle";
static const char *subWindowNameC = "activeSubWindowName";

QMdiAreaPropertySheet::QMdiAreaPropertySheet(QWidget *mdiArea, QObject *parent) :
    QDesignerPropertySheet(mdiArea, parent),
    m_windowTitleProperty(QLatin1String("windowTitle"))
{
    createFakeProperty(QLatin1String(subWindowNameC), QString());
    createFakeProperty(QLatin1String(subWindowTitleC), QString());
}

QMdiAreaPropertySheet::MdiAreaProperty QMdiAreaPropertySheet::mdiAreaProperty(const QString &name)
{
    typedef QHash<QString, MdiAreaProperty> MdiAreaPropertyHash;
    static MdiAreaPropertyHash mdiAreaPropertyHash;
    if (mdiAreaPropertyHash.empty()) {
        mdiAreaPropertyHash.insert(QLatin1String(subWindowNameC), MdiAreaSubWindowName);
        mdiAreaPropertyHash.insert(QLatin1String(subWindowTitleC), MdiAreaSubWindowTitle);
    }
    return mdiAreaPropertyHash.value(name,MdiAreaNone);
}

void QMdiAreaPropertySheet::setProperty(int index, const QVariant &value)
{
    switch (mdiAreaProperty(propertyName(index))) {
    case MdiAreaSubWindowName:
        if (QWidget *w = currentWindow())
            w->setObjectName(value.toString());
        break;
    case MdiAreaSubWindowTitle:        // Forward to window title of subwindow
        if (QDesignerPropertySheetExtension *cws = currentWindowSheet()) {
            const int index = cws->indexOf(m_windowTitleProperty);
            cws->setProperty(index, value);
            cws->setChanged(index, true);
        }
        break;
    default:
        QDesignerPropertySheet::setProperty(index, value);
        break;
    }
}

bool QMdiAreaPropertySheet::reset(int index)
{
    bool rc = true;
    switch (mdiAreaProperty(propertyName(index))) {
    case MdiAreaSubWindowName:
        setProperty(index, QVariant(QString()));
        setChanged(index, false);
        break;
    case MdiAreaSubWindowTitle:        // Forward to window title of subwindow
        if (QDesignerPropertySheetExtension *cws = currentWindowSheet()) {
            const int index = cws->indexOf(m_windowTitleProperty);
            rc = cws->reset(index);
        }
        break;
    default:
        rc = QDesignerPropertySheet::reset(index);
        break;
    }
    return rc;
}

QVariant QMdiAreaPropertySheet::property(int index) const
{
    switch (mdiAreaProperty(propertyName(index))) {
    case MdiAreaSubWindowName:
        if (QWidget *w = currentWindow())
            return w->objectName();
        return QVariant(QString());
    case MdiAreaSubWindowTitle:
        if (QWidget *w = currentWindow())
            return w->windowTitle();
        return QVariant(QString());
    case MdiAreaNone:
        break;
    }
    return QDesignerPropertySheet::property(index);
}

bool QMdiAreaPropertySheet::isEnabled(int index) const
{
    switch (mdiAreaProperty(propertyName(index))) {
    case MdiAreaSubWindowName:
    case MdiAreaSubWindowTitle:
        return currentWindow() != 0;
    case MdiAreaNone:
        break;
    }
    return QDesignerPropertySheet::isEnabled(index);
}

bool QMdiAreaPropertySheet::isChanged(int index) const
{
    bool rc = false;
    switch (mdiAreaProperty(propertyName(index))) {
    case MdiAreaSubWindowName:
        rc = currentWindow() != 0;
        break;
    case MdiAreaSubWindowTitle:
        if (QDesignerPropertySheetExtension *cws = currentWindowSheet()) {
            const int index = cws->indexOf(m_windowTitleProperty);
            rc = cws->isChanged(index);
        }
        break;
    default:
        rc = QDesignerPropertySheet::isChanged(index);
        break;
    }
    return rc;
}

QWidget *QMdiAreaPropertySheet::currentWindow() const
{
    if (const QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension*>(core()->extensionManager(), object())) {
        const int ci = c->currentIndex();
        if (ci < 0)
            return 0;
        return c->widget(ci);
    }
    return 0;
}

QDesignerPropertySheetExtension *QMdiAreaPropertySheet::currentWindowSheet() const
{
    QWidget *cw = currentWindow();
    if (cw == 0)
        return 0;
    return qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), cw);
}

bool QMdiAreaPropertySheet::checkProperty(const QString &propertyName)
{
    return mdiAreaProperty(propertyName) == MdiAreaNone;
}
}
QT_END_NAMESPACE
