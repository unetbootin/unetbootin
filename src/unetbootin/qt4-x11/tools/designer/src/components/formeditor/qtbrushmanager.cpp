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

#include "qtbrushmanager.h"
#include <QtGui/QPixmap>
#include <QtGui/QPainter>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

class QtBrushManagerPrivate
{
    QtBrushManager *q_ptr;
    Q_DECLARE_PUBLIC(QtBrushManager)
public:
    QMap<QString, QBrush> theBrushMap;
    QString theCurrentBrush;
};

QtBrushManager::QtBrushManager(QObject *parent)
    : QDesignerBrushManagerInterface(parent)
{
    d_ptr = new QtBrushManagerPrivate;
    d_ptr->q_ptr = this;

}

QtBrushManager::~QtBrushManager()
{
    delete d_ptr;
}

QBrush QtBrushManager::brush(const QString &name) const
{
    if (d_ptr->theBrushMap.contains(name))
        return d_ptr->theBrushMap[name];
    return QBrush();
}

QMap<QString, QBrush> QtBrushManager::brushes() const
{
    return d_ptr->theBrushMap;
}

QString QtBrushManager::currentBrush() const
{
    return d_ptr->theCurrentBrush;
}

QString QtBrushManager::addBrush(const QString &name, const QBrush &brush)
{
    if (name.isNull())
        return QString();

    QString newName = name;
    QString nameBase = newName;
    int i = 0;
    while (d_ptr->theBrushMap.contains(newName)) {
        newName = nameBase + QString::number(++i);
    }
    d_ptr->theBrushMap[newName] = brush;
    emit brushAdded(newName, brush);

    return newName;
}

void QtBrushManager::removeBrush(const QString &name)
{
    if (!d_ptr->theBrushMap.contains(name))
        return;
    if (currentBrush() == name)
        setCurrentBrush(QString());
    emit brushRemoved(name);
    d_ptr->theBrushMap.remove(name);
}

void QtBrushManager::setCurrentBrush(const QString &name)
{
    QBrush newBrush;
    if (!name.isNull()) {
        if (d_ptr->theBrushMap.contains(name))
            newBrush = d_ptr->theBrushMap[name];
        else
            return;
    }
    d_ptr->theCurrentBrush = name;
    emit currentBrushChanged(name, newBrush);
}

QPixmap QtBrushManager::brushPixmap(const QBrush &brush) const
{
    int w = 64;
    int h = 64;

    QImage img(w, h, QImage::Format_ARGB32_Premultiplied);
    QPainter p(&img);
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.fillRect(QRect(0, 0, w, h), brush);
    return QPixmap::fromImage(img);
}

}  // namespace qdesigner_internal

QT_END_NAMESPACE
