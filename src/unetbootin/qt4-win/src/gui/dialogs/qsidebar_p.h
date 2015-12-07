/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QSIDEBAR_H
#define QSIDEBAR_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qlistwidget.h>
#include <qstandarditemmodel.h>
#include <qurl.h>

#ifndef QT_NO_FILEDIALOG

QT_BEGIN_NAMESPACE

class QFileSystemModel;
class Q_AUTOTEST_EXPORT QUrlModel : public QStandardItemModel
{
    Q_OBJECT

public:
    enum Roles {
        UrlRole = Qt::UserRole + 1
    };

    QUrlModel(QObject *parent = 0);

    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
#ifndef QT_NO_DRAGANDDROP
    bool canDrop(QDragEnterEvent *event);
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
#endif
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);

    void setUrls(const QList<QUrl> &list);
    void addUrls(const QList<QUrl> &urls, int row = -1, bool move = true);
    QList<QUrl> urls() const;
    void setFileSystemModel(QFileSystemModel *model);
    bool showFullPath;

private Q_SLOTS:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void layoutChanged();

private:
    void setUrl(const QModelIndex &index, const QUrl &url, const QModelIndex &dirIndex);
    void changed(const QString &path);
    void addIndexToWatch(const QString &path, const QModelIndex &index);
    QFileSystemModel *fileSystemModel;
    QList<QPair<QModelIndex, QString> > watching;
    QList<QUrl> invalidUrls;
};

class Q_AUTOTEST_EXPORT QSidebar : public QListView
{
    Q_OBJECT

Q_SIGNALS:
    void goToUrl(const QUrl &url);

public:
    QSidebar(QWidget *parent = 0);
    void init(QFileSystemModel *model, const QList<QUrl> &newUrls);
    ~QSidebar();

    QSize sizeHint() const;

    void setUrls(const QList<QUrl> &list) { urlModel->setUrls(list); }
    void addUrls(const QList<QUrl> &list, int row) { urlModel->addUrls(list, row); }
    QList<QUrl> urls() const { return urlModel->urls(); }

    void selectUrl(const QUrl &url);

protected:
    bool event(QEvent * e);
    void focusInEvent(QFocusEvent *event);
#ifndef QT_NO_DRAGANDDROP
    void dragEnterEvent(QDragEnterEvent *event);
#endif

private Q_SLOTS:
    void clicked(const QModelIndex &index);
#ifndef QT_NO_MENU
    void showContextMenu(const QPoint &position);
#endif
    void removeEntry();

private:
    QUrlModel *urlModel;
};

QT_END_NAMESPACE

#endif // QT_NO_FILEDIALOG

#endif // QSIDEBAR_H

