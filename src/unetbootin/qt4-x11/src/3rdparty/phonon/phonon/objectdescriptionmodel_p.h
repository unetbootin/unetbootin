/*  This file is part of the KDE project
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Trolltech ASA 
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef PHONON_OBJECTDESCRIPTIONMODEL_P_H
#define PHONON_OBJECTDESCRIPTIONMODEL_P_H

#include "objectdescriptionmodel.h"
#include <QtCore/QList>
#include "objectdescription.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_OBJECTDESCRIPTIONMODEL

namespace Phonon
{

class ListModelHelper : public QAbstractListModel
{
    public:
        using QAbstractListModel::layoutAboutToBeChanged;
        using QAbstractListModel::changePersistentIndexList;
        using QAbstractListModel::layoutChanged;
        using QAbstractListModel::reset;
        using QAbstractListModel::beginInsertRows;
        using QAbstractListModel::endInsertRows;
        using QAbstractListModel::beginRemoveRows;
        using QAbstractListModel::endRemoveRows;
};

class ObjectDescriptionModelDataPrivate
{
    public:
        ObjectDescriptionModelDataPrivate(QAbstractListModel *m) : model(reinterpret_cast<ListModelHelper *>(m)) {}
        QList<QExplicitlySharedDataPointer<ObjectDescriptionData> > data;
        ListModelHelper *model;
};

}

#endif //QT_NO_PHONON_OBJECTDESCRIPTIONMODEL

QT_END_NAMESPACE

#endif // PHONON_OBJECTDESCRIPTIONMODEL_P_H
// vim: sw=4 ts=4 tw=80
