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

#include "iconcache.h"
#include <QtGui/QPixmap>
#include <QtGui/QIcon>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

using namespace qdesigner_internal;

IconCache::IconCache(QObject *parent)
    : QDesignerIconCacheInterface(parent)
{
}

QIcon IconCache::nameToIcon(const QString &path, const QString &resourcePath)
{
    Q_UNUSED(path)
    Q_UNUSED(resourcePath)
    qWarning() << "IconCache::nameToIcon(): IconCache is obsoleted";
    return QIcon();
}

QString IconCache::iconToFilePath(const QIcon &pm) const
{
    Q_UNUSED(pm)
    qWarning() << "IconCache::iconToFilePath(): IconCache is obsoleted";
    return QString();
}

QString IconCache::iconToQrcPath(const QIcon &pm) const
{
    Q_UNUSED(pm)
    qWarning() << "IconCache::iconToQrcPath(): IconCache is obsoleted";
    return QString();
}

QPixmap IconCache::nameToPixmap(const QString &path, const QString &resourcePath)
{
    Q_UNUSED(path)
    Q_UNUSED(resourcePath)
    qWarning() << "IconCache::nameToPixmap(): IconCache is obsoleted";
    return QPixmap();
}

QString IconCache::pixmapToFilePath(const QPixmap &pm) const
{
    Q_UNUSED(pm)
    qWarning() << "IconCache::pixmapToFilePath(): IconCache is obsoleted";
    return QString();
}

QString IconCache::pixmapToQrcPath(const QPixmap &pm) const
{
    Q_UNUSED(pm)
    qWarning() << "IconCache::pixmapToQrcPath(): IconCache is obsoleted";
    return QString();
}

QList<QPixmap> IconCache::pixmapList() const
{
    qWarning() << "IconCache::pixmapList(): IconCache is obsoleted";
    return QList<QPixmap>();
}

QList<QIcon> IconCache::iconList() const
{
    qWarning() << "IconCache::iconList(): IconCache is obsoleted";
    return QList<QIcon>();
}

QString IconCache::resolveQrcPath(const QString &filePath, const QString &qrcPath, const QString &wd) const
{
    Q_UNUSED(filePath)
    Q_UNUSED(qrcPath)
    Q_UNUSED(wd)
    qWarning() << "IconCache::resolveQrcPath(): IconCache is obsoleted";
    return QString();
}

QT_END_NAMESPACE
