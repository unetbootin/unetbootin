/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#ifndef QHELPCONTENTWIDGET_H
#define QHELPCONTENTWIDGET_H

#include <QtHelp/qhelp_global.h>

#include <QtCore/QQueue>
#include <QtCore/QString>
#include <QtGui/QTreeView>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Help)

class QHelpEnginePrivate;
class QHelpDBReader;
class QHelpContentItemPrivate;
class QHelpContentModelPrivate;
class QHelpEngine;
class QHelpContentProvider;

class QHELP_EXPORT QHelpContentItem
{
public:
    ~QHelpContentItem();

    QHelpContentItem *child(int row) const;
    int childCount() const;
    QString title() const;
    QUrl url() const;
    int row() const;
    QHelpContentItem *parent() const;
    int childPosition(QHelpContentItem *child) const;

private:
    QHelpContentItem(const QString &name, const QString &link,
        QHelpDBReader *reader, QHelpContentItem *parent = 0);
    void appendChild(QHelpContentItem *child);

    QHelpContentItemPrivate *d;
    friend class QHelpContentProvider;
};

class QHELP_EXPORT QHelpContentModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    ~QHelpContentModel();

    void createContents(const QString &customFilterName);
    QHelpContentItem *contentItemAt(const QModelIndex &index) const;

    QVariant data(const QModelIndex &index, int role) const;
    QModelIndex index(int row, int column,
        const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool isCreatingContents() const;

Q_SIGNALS:
    void contentsCreationStarted();
    void contentsCreated();

private Q_SLOTS:
    void insertContents();
    void invalidateContents(bool onShutDown = false);

private:
    QHelpContentModel(QHelpEnginePrivate *helpEngine);
    QHelpContentModelPrivate *d;
    friend class QHelpEnginePrivate;
};

class QHELP_EXPORT QHelpContentWidget : public QTreeView
{
    Q_OBJECT

public:
    QModelIndex indexOf(const QUrl &link);

Q_SIGNALS:
    void linkActivated(const QUrl &link);

private Q_SLOTS:
    void showLink(const QModelIndex &index);

private:
    bool searchContentItem(QHelpContentModel *model,
        const QModelIndex &parent, const QString &path);
    QModelIndex m_syncIndex;

private:
    QHelpContentWidget();
    friend class QHelpEngine;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif

