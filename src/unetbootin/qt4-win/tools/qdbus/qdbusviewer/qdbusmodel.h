/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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

#ifndef QDBUSMODEL_H
#define QDBUSMODEL_H

#include <QtCore/qabstractitemmodel.h>
#include <QtDBus/QtDBus>

struct QDBusItem;

QT_FORWARD_DECLARE_CLASS(QDomDocument);
QT_FORWARD_DECLARE_CLASS(QDomElement);


class QDBusModel: public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Type { InterfaceItem, PathItem, MethodItem, SignalItem, PropertyItem };

    QDBusModel(const QString &service, const QDBusConnection &connection);
    ~QDBusModel();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    Type itemType(const QModelIndex &index) const;
    QString dBusPath(const QModelIndex &index) const;
    QString dBusInterface(const QModelIndex &index) const;
    QString dBusMethodName(const QModelIndex &index) const;

    void refresh(const QModelIndex &index = QModelIndex());

    QModelIndex findObject(const QDBusObjectPath &objectPath);

Q_SIGNALS:
    void busError(const QString &text);

private:
    QDomDocument introspect(const QString &path);
    void addMethods(QDBusItem *parent, const QDomElement &iface);
    void addPath(QDBusItem *parent);

    QString service;
    QDBusConnection c;
    QDBusItem *root;
};

#endif

