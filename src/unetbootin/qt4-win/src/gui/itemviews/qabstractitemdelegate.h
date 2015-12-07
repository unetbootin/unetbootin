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

#ifndef QABSTRACTITEMDELEGATE_H
#define QABSTRACTITEMDELEGATE_H

#include <QtCore/qobject.h>
#include <QtGui/qstyleoption.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_ITEMVIEWS

class QPainter;
class QModelIndex;
class QAbstractItemModel;
class QAbstractItemView;
class QHelpEvent;

class Q_GUI_EXPORT QAbstractItemDelegate : public QObject
{
    Q_OBJECT

public:

    enum EndEditHint {
        NoHint,
        EditNextItem,
        EditPreviousItem,
        SubmitModelCache,
        RevertModelCache
    };

    explicit QAbstractItemDelegate(QObject *parent = 0);
    virtual ~QAbstractItemDelegate();

    // painting
    virtual void paint(QPainter *painter,
                       const QStyleOptionViewItem &option,
                       const QModelIndex &index) const = 0;

    virtual QSize sizeHint(const QStyleOptionViewItem &option,
                           const QModelIndex &index) const = 0;

    // editing
    virtual QWidget *createEditor(QWidget *parent,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const;

    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;

    virtual void setModelData(QWidget *editor,
                              QAbstractItemModel *model,
                              const QModelIndex &index) const;

    virtual void updateEditorGeometry(QWidget *editor,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const;

    // for non-widget editors
    virtual bool editorEvent(QEvent *event,
                             QAbstractItemModel *model,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index);

    static QString elidedText(const QFontMetrics &fontMetrics, int width,
                              Qt::TextElideMode mode, const QString &text);

public Q_SLOTS:
    bool helpEvent(QHelpEvent *event,
                   QAbstractItemView *view,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index);

Q_SIGNALS:
    void commitData(QWidget *editor);
    void closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint = NoHint);
    void sizeHintChanged(const QModelIndex &);

protected:
    QAbstractItemDelegate(QObjectPrivate &, QObject *parent = 0);
private:
    Q_DISABLE_COPY(QAbstractItemDelegate)
};

#endif // QT_NO_ITEMVIEWS

QT_END_NAMESPACE

QT_END_HEADER

#endif // QABSTRACTITEMDELEGATE_H
