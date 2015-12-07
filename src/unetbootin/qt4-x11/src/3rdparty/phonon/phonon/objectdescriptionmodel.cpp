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

#include "objectdescriptionmodel.h"
#include "objectdescriptionmodel_p.h"
#include "phonondefs_p.h"
#include "platform_p.h"
#include <QtCore/QList>
#include "objectdescription.h"
#include "phononnamespace_p.h"
#include <QtCore/QMimeData>
#include <QtCore/QStringList>
#include <QtGui/QIcon>
#include "factory_p.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_OBJECTDESCRIPTIONMODEL


static const uint qt_meta_data_Phonon__ObjectDescriptionModel[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_Phonon__ObjectDescriptionModel_AudioOutputDeviceType[]  = { "Phonon::AudioOutputDevice\0" };
static const char qt_meta_stringdata_Phonon__ObjectDescriptionModel_AudioCaptureDeviceType[] = { "Phonon::AudioCaptureDevice\0" };
static const char qt_meta_stringdata_Phonon__ObjectDescriptionModel_EffectType[]             = { "Phonon::EffectDescription\0" };
static const char qt_meta_stringdata_Phonon__ObjectDescriptionModel_AudioChannelType[]        = { "Phonon::AudioChannelDescription\0" };
static const char qt_meta_stringdata_Phonon__ObjectDescriptionModel_SubtitleType[]     = { "Phonon::SubtitleDescription\0" };
/*
static const char qt_meta_stringdata_Phonon__ObjectDescriptionModel_VideoOutputDeviceType[]  = { "Phonon::VideoOutputDevice\0" };
static const char qt_meta_stringdata_Phonon__ObjectDescriptionModel_VideoCaptureDeviceType[] = { "Phonon::VideoCaptureDevice\0" };
static const char qt_meta_stringdata_Phonon__ObjectDescriptionModel_AudioCodecType[]         = { "Phonon::AudioCodecDescription\0" };
static const char qt_meta_stringdata_Phonon__ObjectDescriptionModel_VideoCodecType[]         = { "Phonon::VideoCodecDescription\0" };
static const char qt_meta_stringdata_Phonon__ObjectDescriptionModel_ContainerFormatType[]    = { "Phonon::ContainerFormatDescription\0" };
static const char qt_meta_stringdata_Phonon__ObjectDescriptionModel_VisualizationType[]      = { "Phonon::VisualizationDescription\0" };
*/

namespace Phonon
{

template<> const QMetaObject ObjectDescriptionModel<AudioOutputDeviceType>::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_Phonon__ObjectDescriptionModel_AudioOutputDeviceType,
      qt_meta_data_Phonon__ObjectDescriptionModel, 0 }
};
template<> const QMetaObject ObjectDescriptionModel<EffectType>::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_Phonon__ObjectDescriptionModel_EffectType,
      qt_meta_data_Phonon__ObjectDescriptionModel, 0 }
};
template<> const QMetaObject ObjectDescriptionModel<AudioChannelType>::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_Phonon__ObjectDescriptionModel_AudioChannelType,
      qt_meta_data_Phonon__ObjectDescriptionModel, 0 }
};
template<> const QMetaObject ObjectDescriptionModel<SubtitleType>::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_Phonon__ObjectDescriptionModel_SubtitleType,
      qt_meta_data_Phonon__ObjectDescriptionModel, 0 }
};
template<> const QMetaObject ObjectDescriptionModel<AudioCaptureDeviceType>::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_Phonon__ObjectDescriptionModel_AudioCaptureDeviceType,
      qt_meta_data_Phonon__ObjectDescriptionModel, 0 }
};
/*template<> const QMetaObject ObjectDescriptionModel<VideoOutputDeviceType>::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_Phonon__ObjectDescriptionModel_VideoOutputDeviceType,
      qt_meta_data_Phonon__ObjectDescriptionModel, 0 }
};
template<> const QMetaObject ObjectDescriptionModel<VideoCaptureDeviceType>::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_Phonon__ObjectDescriptionModel_VideoCaptureDeviceType,
      qt_meta_data_Phonon__ObjectDescriptionModel, 0 }
};
template<> const QMetaObject ObjectDescriptionModel<AudioCodecType>::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_Phonon__ObjectDescriptionModel_AudioCodecType,
      qt_meta_data_Phonon__ObjectDescriptionModel, 0 }
};
template<> const QMetaObject ObjectDescriptionModel<VideoCodecType>::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_Phonon__ObjectDescriptionModel_VideoCodecType,
      qt_meta_data_Phonon__ObjectDescriptionModel, 0 }
};
template<> const QMetaObject ObjectDescriptionModel<ContainerFormatType>::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_Phonon__ObjectDescriptionModel_ContainerFormatType,
      qt_meta_data_Phonon__ObjectDescriptionModel, 0 }
};
template<> const QMetaObject ObjectDescriptionModel<VisualizationType>::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_Phonon__ObjectDescriptionModel_VisualizationType,
      qt_meta_data_Phonon__ObjectDescriptionModel, 0 }
};*/

template<ObjectDescriptionType type>
const QMetaObject *ObjectDescriptionModel<type>::metaObject() const
{
    return &staticMetaObject;
}

template<ObjectDescriptionType type>
void *ObjectDescriptionModel<type>::qt_metacast(const char *_clname)
{
    if (!_clname) {
        return 0;
    }
    if (!strcmp(_clname, ObjectDescriptionModel<type>::staticMetaObject.className())) {
        return static_cast<void *>(const_cast<ObjectDescriptionModel<type> *>(this));
    }
    return QAbstractListModel::qt_metacast(_clname);
}

/*
template<ObjectDescriptionType type>
int ObjectDescriptionModel<type>::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    return QAbstractListModel::qt_metacall(_c, _id, _a);
}
*/

int ObjectDescriptionModelData::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return d->data.size();
}

QVariant ObjectDescriptionModelData::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= d->data.size() || index.column() != 0)
        return QVariant();

    switch(role)
    {
    case Qt::EditRole:
    case Qt::DisplayRole:
        return d->data.at(index.row())->name();
        break;
    case Qt::ToolTipRole:
        return d->data.at(index.row())->description();
        break;
    case Qt::DecorationRole:
        {
            QVariant icon = d->data.at(index.row())->property("icon");
            if (icon.isValid()) {
                if (icon.type() == QVariant::String) {
                    return Platform::icon(icon.toString());
                } else if (icon.type() == QVariant::Icon) {
                    return icon;
                }
            }
        }
        return QVariant();
    default:
        return QVariant();
}
}

Qt::ItemFlags ObjectDescriptionModelData::flags(const QModelIndex &index) const
{
    if(!index.isValid() || index.row() >= d->data.size() || index.column() != 0) {
        return Qt::ItemIsDropEnabled;
    }

    QVariant available = d->data.at(index.row())->property("available");
    if (available.isValid() && available.type() == QVariant::Bool && !available.toBool()) {
        return Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
    }
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled;
}

QList<int> ObjectDescriptionModelData::tupleIndexOrder() const
{
    QList<int> ret;
    for (int i = 0; i < d->data.size(); ++i) {
        ret.append(d->data.at(i)->index());
    }
    return ret;
}

int ObjectDescriptionModelData::tupleIndexAtPositionIndex(int positionIndex) const
{
    return d->data.at(positionIndex)->index();
}

QMimeData *ObjectDescriptionModelData::mimeData(ObjectDescriptionType type, const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData;
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    QModelIndexList::const_iterator end = indexes.constEnd();
    QModelIndexList::const_iterator index = indexes.constBegin();
    for(; index!=end; ++index) {
        if ((*index).isValid()) {
            stream << d->data.at((*index).row())->index();
        }
    }
    //pDebug() << Q_FUNC_INFO << "setting mimeData to" << mimeTypes(type).first() << "=>" << encodedData.toHex();
    mimeData->setData(mimeTypes(type).first(), encodedData);
    return mimeData;
}

void ObjectDescriptionModelData::moveUp(const QModelIndex &index)
{
    if (!index.isValid() || index.row() >= d->data.size() || index.row() < 1 || index.column() != 0)
        return;

    emit d->model->layoutAboutToBeChanged();
    QModelIndex above = index.sibling(index.row() - 1, index.column());
    d->data.swap(index.row(), above.row());
    QModelIndexList from, to;
    from << index << above;
    to << above << index;
    d->model->changePersistentIndexList(from, to);
    emit d->model->layoutChanged();
}

void ObjectDescriptionModelData::moveDown(const QModelIndex &index)
{
    if (!index.isValid() || index.row() >= d->data.size() - 1 || index.column() != 0)
        return;

    emit d->model->layoutAboutToBeChanged();
    QModelIndex below = index.sibling(index.row() + 1, index.column());
    d->data.swap(index.row(), below.row());
    QModelIndexList from, to;
    from << index << below;
    to << below << index;
    d->model->changePersistentIndexList(from, to);
    emit d->model->layoutChanged();
}

ObjectDescriptionModelData::ObjectDescriptionModelData(QAbstractListModel *model)
    : d(new ObjectDescriptionModelDataPrivate(model))
{
}

ObjectDescriptionModelData::~ObjectDescriptionModelData()
{
    delete d;
}

void ObjectDescriptionModelData::setModelData(const QList<QExplicitlySharedDataPointer<ObjectDescriptionData> > &newData)
{
    d->data = newData;
    d->model->reset();
}

QList<QExplicitlySharedDataPointer<ObjectDescriptionData> > ObjectDescriptionModelData::modelData() const
{
    return d->data;
}

QExplicitlySharedDataPointer<ObjectDescriptionData> ObjectDescriptionModelData::modelData(const QModelIndex &index) const
{
    if (!index.isValid() || index.row() >= d->data.size() || index.column() != 0) {
        return QExplicitlySharedDataPointer<ObjectDescriptionData>(new ObjectDescriptionData(0));
    }
    return d->data.at(index.row());
}

Qt::DropActions ObjectDescriptionModelData::supportedDropActions() const
{
    //pDebug() << Q_FUNC_INFO;
    return Qt::MoveAction;
}

bool ObjectDescriptionModelData::dropMimeData(ObjectDescriptionType type, const QMimeData *data, Qt::DropAction action,
        int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(action);
    Q_UNUSED(column);
    Q_UNUSED(parent);
    //pDebug() << Q_FUNC_INFO << data << action << row << column << parent;

    QString format = mimeTypes(type).first();
    if (!data->hasFormat(format)) {
        return false;
    }

    if (row == -1) {
        row = d->data.size();
    }

    QByteArray encodedData = data->data(format);
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QList<QExplicitlySharedDataPointer<ObjectDescriptionData> > toInsert;
    while (!stream.atEnd()) {
        int otherIndex;
        stream >> otherIndex;
        ObjectDescriptionData *obj = ObjectDescriptionData::fromIndex(type, otherIndex);

        if (obj->isValid()) {
            toInsert << QExplicitlySharedDataPointer<ObjectDescriptionData>(obj);
        } else {
            delete obj;
        }
    }
    d->model->beginInsertRows(QModelIndex(), row, row + toInsert.size() - 1);
    foreach (const QExplicitlySharedDataPointer<ObjectDescriptionData> &obj, toInsert) {
        d->data.insert(row, obj);
    }
    d->model->endInsertRows();
    return true;
}


bool ObjectDescriptionModelData::removeRows(int row, int count, const QModelIndex &parent)
{
    //pDebug() << Q_FUNC_INFO << row << count << parent;
    if (parent.isValid() || row + count > d->data.size()) {
        return false;
    }
    d->model->beginRemoveRows(parent, row, row + count - 1);
    for (;count > 0; --count) {
        d->data.removeAt(row);
    }
    d->model->endRemoveRows();
    return true;
}

/*
template<ObjectDescriptionType type>
bool ObjectDescriptionModel<type>::insertRows(int row, int count, const QModelIndex &parent)
{
    pDebug() << Q_FUNC_INFO << row << count << parent;
    if (parent.isValid() || row < 0 || row > d->data.size()) {
        return false;
    }
    beginInsertRows(parent, row, row + count - 1);
    for (;count > 0; --count) {
        d->data.insert(row, ObjectDescription<type>());
    }
    endInsertRows();
    return true;
}
*/

QStringList ObjectDescriptionModelData::mimeTypes(ObjectDescriptionType type) const
{
    return QStringList(QLatin1String("application/x-phonon-objectdescription") + QString::number(static_cast<int>(type)));
}

#if !defined(Q_CC_MSVC) || _MSC_VER > 1300 || defined(Q_CC_INTEL) || defined(Q_CC_MINGW)
#define INSTANTIATE_META_FUNCTIONS(type) \
template const QMetaObject *ObjectDescriptionModel<type>::metaObject() const; \
template void *ObjectDescriptionModel<type>::qt_metacast(const char *)

INSTANTIATE_META_FUNCTIONS(AudioOutputDeviceType);
INSTANTIATE_META_FUNCTIONS(AudioCaptureDeviceType);
INSTANTIATE_META_FUNCTIONS(EffectType);
INSTANTIATE_META_FUNCTIONS(AudioChannelType);
INSTANTIATE_META_FUNCTIONS(SubtitleType);
#endif
/*INSTANTIATE_META_FUNCTIONS(VideoOutputDeviceType);
INSTANTIATE_META_FUNCTIONS(VideoCaptureDeviceType);
INSTANTIATE_META_FUNCTIONS(AudioCodecType);
INSTANTIATE_META_FUNCTIONS(VideoCodecType);
INSTANTIATE_META_FUNCTIONS(ContainerFormatType);
INSTANTIATE_META_FUNCTIONS(VisualizationType);
*/
} // namespace Phonon

#endif //QT_NO_PHONON_OBJECTDESCRIPTIONMODEL

QT_END_NAMESPACE
