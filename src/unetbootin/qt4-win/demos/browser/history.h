/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#ifndef HISTORY_H
#define HISTORY_H

#include "modelmenu.h"

#include <QtCore/QDateTime>
#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QUrl>

#include <QtGui/QSortFilterProxyModel>

#include <QWebHistoryInterface>

class HistoryItem
{
public:
    HistoryItem() {}
    HistoryItem(const QString &u,
                const QDateTime &d = QDateTime(), const QString &t = QString())
        : title(t), url(u), dateTime(d) {}

    inline bool operator==(const HistoryItem &other) const
        { return other.title == title
          && other.url == url && other.dateTime == dateTime; }

    // history is sorted in reverse
    inline bool operator <(const HistoryItem &other) const
        { return dateTime > other.dateTime; }

    QString title;
    QString url;
    QDateTime dateTime;
};

class AutoSaver;
class HistoryModel;
class HistoryFilterModel;
class HistoryTreeModel;
class HistoryManager : public QWebHistoryInterface
{
    Q_OBJECT
    Q_PROPERTY(int historyLimit READ historyLimit WRITE setHistoryLimit)

signals:
    void historyReset();
    void entryAdded(const HistoryItem &item);
    void entryRemoved(const HistoryItem &item);
    void entryUpdated(int offset);

public:
    HistoryManager(QObject *parent = 0);
    ~HistoryManager();

    bool historyContains(const QString &url) const;
    void addHistoryEntry(const QString &url);

    void updateHistoryItem(const QUrl &url, const QString &title);

    int historyLimit() const;
    void setHistoryLimit(int limit);

    QList<HistoryItem> history() const;
    void setHistory(const QList<HistoryItem> &history, bool loadedAndSorted = false);

    // History manager keeps around these models for use by the completer and other classes
    HistoryModel *historyModel() const;
    HistoryFilterModel *historyFilterModel() const;
    HistoryTreeModel *historyTreeModel() const;

public slots:
    void clear();
    void loadSettings();

private slots:
    void save();
    void checkForExpired();

protected:
    void addHistoryItem(const HistoryItem &item);

private:
    void load();

    AutoSaver *m_saveTimer;
    int m_historyLimit;
    QTimer m_expiredTimer;
    QList<HistoryItem> m_history;
    QString m_lastSavedUrl;

    HistoryModel *m_historyModel;
    HistoryFilterModel *m_historyFilterModel;
    HistoryTreeModel *m_historyTreeModel;
};

class HistoryModel : public QAbstractTableModel
{
    Q_OBJECT

public slots:
    void historyReset();
    void entryAdded();
    void entryUpdated(int offset);

public:
    enum Roles {
        DateRole = Qt::UserRole + 1,
        DateTimeRole = Qt::UserRole + 2,
        UrlRole = Qt::UserRole + 3,
        UrlStringRole = Qt::UserRole + 4
    };

    HistoryModel(HistoryManager *history, QObject *parent = 0);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

private:
    HistoryManager *m_history;
};

/*!
    Proxy model that will remove any duplicate entries.
    Both m_sourceRow and m_historyHash store their offsets not from
    the front of the list, but as offsets from the back.
  */
class HistoryFilterModel : public QAbstractProxyModel
{
    Q_OBJECT

public:
    HistoryFilterModel(QAbstractItemModel *sourceModel, QObject *parent = 0);

    inline bool historyContains(const QString &url) const
        { load(); return m_historyHash.contains(url); }
    int historyLocation(const QString &url) const;

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    void setSourceModel(QAbstractItemModel *sourceModel);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(int, int, const QModelIndex& = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& index= QModelIndex()) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private slots:
    void sourceReset();
    void sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void sourceRowsInserted(const QModelIndex &parent, int start, int end);
    void sourceRowsRemoved(const QModelIndex &, int, int);

private:
    void load() const;

    mutable QList<int> m_sourceRow;
    mutable QHash<QString, int> m_historyHash;
    mutable bool m_loaded;
};

/*
    The history menu
    - Removes the first twenty entries and puts them as children of the top level.
    - If there are less then twenty entries then the first folder is also removed.

    The mapping is done by knowing that HistoryTreeModel is over a table
    We store that row offset in our index's private data.
*/
class HistoryMenuModel : public QAbstractProxyModel
{
    Q_OBJECT

public:
    HistoryMenuModel(HistoryTreeModel *sourceModel, QObject *parent = 0);
    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex mapFromSource(const QModelIndex & sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex & proxyIndex) const;
    QModelIndex index(int, int, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index = QModelIndex()) const;

    int bumpedRows() const;

private:
    HistoryTreeModel *m_treeModel;
};

// Menu that is dynamically populated from the history
class HistoryMenu : public ModelMenu
{
    Q_OBJECT

signals:
    void openUrl(const QUrl &url);

public:
     HistoryMenu(QWidget *parent = 0);
     void setInitialActions(QList<QAction*> actions);

protected:
    bool prePopulated();
    void postPopulated();

private slots:
    void activated(const QModelIndex &index);
    void showHistoryDialog();

private:
    HistoryManager *m_history;
    HistoryMenuModel *m_historyMenuModel;
    QList<QAction*> m_initialActions;
};

// proxy model for the history model that
// exposes each url http://www.foo.com and it url starting at the host www.foo.com
class HistoryCompletionModel : public QAbstractProxyModel
{
    Q_OBJECT

public:
    HistoryCompletionModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    QModelIndex index(int, int, const QModelIndex& = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& index= QModelIndex()) const;
    void setSourceModel(QAbstractItemModel *sourceModel);

private slots:
    void sourceReset();

};

// proxy model for the history model that converts the list
// into a tree, one top level node per day.
// Used in the HistoryDialog.
class HistoryTreeModel : public QAbstractProxyModel
{
    Q_OBJECT

public:
    HistoryTreeModel(QAbstractItemModel *sourceModel, QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index= QModelIndex()) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    void setSourceModel(QAbstractItemModel *sourceModel);

private slots:
    void sourceReset();
    void sourceRowsInserted(const QModelIndex &parent, int start, int end);
    void sourceRowsRemoved(const QModelIndex &parent, int start, int end);

private:
    int sourceDateRow(int row) const;
    mutable QList<int> m_sourceRowCache;

};

// A modified QSortFilterProxyModel that always accepts the root nodes in the tree
// so filtering is only done on the children.
// Used in the HistoryDialog
class TreeProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    TreeProxyModel(QObject *parent = 0);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};

#include "ui_history.h"

class HistoryDialog : public QDialog, public Ui_HistoryDialog
{
    Q_OBJECT

signals:
    void openUrl(const QUrl &url);

public:
    HistoryDialog(QWidget *parent = 0, HistoryManager *history = 0);

private slots:
    void customContextMenuRequested(const QPoint &pos);
    void open();
    void copy();

};

#endif // HISTORY_H

