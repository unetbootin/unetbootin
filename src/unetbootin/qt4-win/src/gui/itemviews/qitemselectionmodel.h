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

#ifndef QITEMSELECTIONMODEL_H
#define QITEMSELECTIONMODEL_H

#include <QtCore/qset.h>
#include <QtCore/qvector.h>
#include <QtCore/qlist.h>
#include <QtCore/qabstractitemmodel.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_ITEMVIEWS

class Q_GUI_EXPORT QItemSelectionRange
{

public:
    inline QItemSelectionRange() {}
    inline QItemSelectionRange(const QItemSelectionRange &other)
        : tl(other.tl), br(other.br) {}
    inline QItemSelectionRange(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    explicit inline QItemSelectionRange(const QModelIndex &index)
        { tl = index; br = tl; }

    inline int top() const { return tl.row(); }
    inline int left() const { return tl.column(); }
    inline int bottom() const { return br.row(); }
    inline int right() const { return br.column(); }
    inline int width() const { return br.column() - tl.column() + 1; }
    inline int height() const { return br.row() - tl.row() + 1; }

    inline QModelIndex topLeft() const { return QModelIndex(tl); }
    inline QModelIndex bottomRight() const { return QModelIndex(br); }
    inline QModelIndex parent() const { return tl.parent(); }
    inline const QAbstractItemModel *model() const { return tl.model(); }

    inline bool contains(const QModelIndex &index) const
    {
        return (parent() == index.parent()
                && tl.row() <= index.row() && tl.column() <= index.column()
                && br.row() >= index.row() && br.column() >= index.column());
    }

    inline bool contains(int row, int column, const QModelIndex &parentIndex) const
    {
        return (parent() == parentIndex
                && tl.row() <= row && tl.column() <= column
                && br.row() >= row && br.column() >= column);
    }

    bool intersects(const QItemSelectionRange &other) const;
    QItemSelectionRange intersect(const QItemSelectionRange &other) const; // ### Qt 5: make QT4_SUPPORT
    inline QItemSelectionRange intersected(const QItemSelectionRange &other) const
        { return intersect(other); }

    inline bool operator==(const QItemSelectionRange &other) const
        { return (tl == other.tl && br == other.br); }
    inline bool operator!=(const QItemSelectionRange &other) const
        { return !operator==(other); }

    inline bool isValid() const
    {
        return (tl.isValid() && br.isValid() && tl.parent() == br.parent()
                && top() <= bottom() && left() <= right());
    }

    QModelIndexList indexes() const;

private:
    QPersistentModelIndex tl, br;
};
Q_DECLARE_TYPEINFO(QItemSelectionRange, Q_MOVABLE_TYPE);

inline QItemSelectionRange::QItemSelectionRange(const QModelIndex &atopLeft,
                                                const QModelIndex &abottomRight)
{ tl = atopLeft; br = abottomRight; }

class QItemSelection;
class QItemSelectionModelPrivate;

class Q_GUI_EXPORT QItemSelectionModel : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QItemSelectionModel)
    Q_FLAGS(SelectionFlags)

public:

    enum SelectionFlag {
        NoUpdate       = 0x0000,
        Clear          = 0x0001,
        Select         = 0x0002,
        Deselect       = 0x0004,
        Toggle         = 0x0008,
        Current        = 0x0010,
        Rows           = 0x0020,
        Columns        = 0x0040,
        SelectCurrent  = Select | Current,
        ToggleCurrent  = Toggle | Current,
        ClearAndSelect = Clear | Select
    };

    Q_DECLARE_FLAGS(SelectionFlags, SelectionFlag)

    explicit QItemSelectionModel(QAbstractItemModel *model);
    explicit QItemSelectionModel(QAbstractItemModel *model, QObject *parent);
    virtual ~QItemSelectionModel();

    QModelIndex currentIndex() const;

    bool isSelected(const QModelIndex &index) const;
    bool isRowSelected(int row, const QModelIndex &parent) const;
    bool isColumnSelected(int column, const QModelIndex &parent) const;

    bool rowIntersectsSelection(int row, const QModelIndex &parent) const;
    bool columnIntersectsSelection(int column, const QModelIndex &parent) const;

    bool hasSelection() const;

    QModelIndexList selectedIndexes() const;
    QModelIndexList selectedRows(int column = 0) const;
    QModelIndexList selectedColumns(int row = 0) const;
    const QItemSelection selection() const;

    const QAbstractItemModel *model() const;

public Q_SLOTS:
    void setCurrentIndex(const QModelIndex &index, QItemSelectionModel::SelectionFlags command);
    virtual void select(const QModelIndex &index, QItemSelectionModel::SelectionFlags command);
    virtual void select(const QItemSelection &selection, QItemSelectionModel::SelectionFlags command);
    virtual void clear();
    virtual void reset();

    void clearSelection();

Q_SIGNALS:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void currentRowChanged(const QModelIndex &current, const QModelIndex &previous);
    void currentColumnChanged(const QModelIndex &current, const QModelIndex &previous);

protected:
    QItemSelectionModel(QItemSelectionModelPrivate &dd, QAbstractItemModel *model);
    void emitSelectionChanged(const QItemSelection &newSelection, const QItemSelection &oldSelection);

private:
    Q_DISABLE_COPY(QItemSelectionModel)
    Q_PRIVATE_SLOT(d_func(), void _q_columnsAboutToBeRemoved(const QModelIndex&, int, int))
    Q_PRIVATE_SLOT(d_func(), void _q_rowsAboutToBeRemoved(const QModelIndex&, int, int))
    Q_PRIVATE_SLOT(d_func(), void _q_columnsAboutToBeInserted(const QModelIndex&, int, int))
    Q_PRIVATE_SLOT(d_func(), void _q_rowsAboutToBeInserted(const QModelIndex&, int, int))
    Q_PRIVATE_SLOT(d_func(), void _q_layoutAboutToBeChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_layoutChanged())
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QItemSelectionModel::SelectionFlags)

// dummy implentation of qHash() necessary for instantiating QList<QItemSelectionRange>::toSet() with MSVC
inline uint qHash(const QItemSelectionRange &) { return 0; }

class Q_GUI_EXPORT QItemSelection : public QList<QItemSelectionRange>
{
public:
    QItemSelection() {}
    QItemSelection(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void select(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    bool contains(const QModelIndex &index) const;
    QModelIndexList indexes() const;
    void merge(const QItemSelection &other, QItemSelectionModel::SelectionFlags command);
    static void split(const QItemSelectionRange &range,
                      const QItemSelectionRange &other,
                      QItemSelection *result);
};

#ifndef QT_NO_DEBUG_STREAM
Q_GUI_EXPORT QDebug operator<<(QDebug, const QItemSelectionRange &);
#endif

#endif // QT_NO_ITEMVIEWS

QT_END_NAMESPACE

QT_END_HEADER

#endif // QITEMSELECTIONMODEL_H
