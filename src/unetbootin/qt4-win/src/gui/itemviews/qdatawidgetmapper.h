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

#ifndef QDATAWIDGETMAPPER_H
#define QDATAWIDGETMAPPER_H

#include "QtCore/qobject.h"

#ifndef QT_NO_DATAWIDGETMAPPER

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QAbstractItemDelegate;
class QAbstractItemModel;
class QModelIndex;
class QDataWidgetMapperPrivate;

class Q_GUI_EXPORT QDataWidgetMapper: public QObject
{
    Q_OBJECT

    Q_ENUMS(SubmitPolicy)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
    Q_PROPERTY(SubmitPolicy submitPolicy READ submitPolicy WRITE setSubmitPolicy)

public:
    QDataWidgetMapper(QObject *parent = 0);
    ~QDataWidgetMapper();

    void setModel(QAbstractItemModel *model);
    QAbstractItemModel *model() const;

    void setItemDelegate(QAbstractItemDelegate *delegate);
    QAbstractItemDelegate *itemDelegate() const;

    void setRootIndex(const QModelIndex &index);
    QModelIndex rootIndex() const;

    void setOrientation(Qt::Orientation aOrientation);
    Qt::Orientation orientation() const;

    enum SubmitPolicy { AutoSubmit, ManualSubmit };
    void setSubmitPolicy(SubmitPolicy policy);
    SubmitPolicy submitPolicy() const;

    void addMapping(QWidget *widget, int section);
    void addMapping(QWidget *widget, int section, const QByteArray &propertyName);
    void removeMapping(QWidget *widget);
    int mappedSection(QWidget *widget) const;
    QByteArray mappedPropertyName(QWidget *widget) const;
    QWidget *mappedWidgetAt(int section) const;
    void clearMapping();

    int currentIndex() const;

public Q_SLOTS:
    void revert();
    bool submit();

    void toFirst();
    void toLast();
    void toNext();
    void toPrevious();
    virtual void setCurrentIndex(int index);
    void setCurrentModelIndex(const QModelIndex &index);

Q_SIGNALS:
    void currentIndexChanged(int index);

private:
    Q_DECLARE_PRIVATE(QDataWidgetMapper)
    Q_DISABLE_COPY(QDataWidgetMapper)
    Q_PRIVATE_SLOT(d_func(), void _q_dataChanged(const QModelIndex &, const QModelIndex &))
    Q_PRIVATE_SLOT(d_func(), void _q_commitData(QWidget *))
    Q_PRIVATE_SLOT(d_func(), void _q_closeEditor(QWidget *, QAbstractItemDelegate::EndEditHint))
    Q_PRIVATE_SLOT(d_func(), void _q_modelDestroyed())
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_DATAWIDGETMAPPER
#endif

