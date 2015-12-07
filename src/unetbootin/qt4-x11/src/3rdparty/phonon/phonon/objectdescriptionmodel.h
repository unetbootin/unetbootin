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

#ifndef PHONON_OBJECTDESCRIPTIONMODEL_H
#define PHONON_OBJECTDESCRIPTIONMODEL_H

#include "phonon_export.h"
#include "phonondefs.h"
#include "objectdescription.h"
#include <QtCore/QList>
#include <QtCore/QModelIndex>
#include <QtCore/QStringList>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_OBJECTDESCRIPTIONMODEL

/* MinGW 3.4.x gives an ICE when trying to instantiate one of the
   ObjectDescriptionModel<foo> classes because it can't handle
   half exported classes correct. gcc 4.3.x has a fix for this but
   we currently there's no official gcc 4.3 on windows available.
   Because of this we need this little hack
 */
#if defined(Q_CC_MINGW)
#define PHONON_EXPORT_ODM
#else
#define PHONON_EXPORT_ODM  PHONON_EXPORT
#endif

namespace Phonon
{
    class ObjectDescriptionModelDataPrivate;

    /** \internal
     * \class ObjectDescriptionModelData objectdescriptionmodel.h Phonon/ObjectDescriptionModelData
     * \brief Data class for models for ObjectDescription objects.
     *
     * \author Matthias Kretz <kretz@kde.org>
     */
    class PHONON_EXPORT ObjectDescriptionModelData
    {
        public:
            /**
             * Returns the number of rows in the model. This value corresponds
             * to the size of the list passed through setModelData.
             *
             * \param parent The optional \p parent argument is used in most models to specify
             * the parent of the rows to be counted. Because this is a list if a
             * valid parent is specified the result will always be 0.
             *
             * Reimplemented from QAbstractItemModel.
             *
             * \see QAbstractItemModel::rowCount
             */
            int rowCount(const QModelIndex &parent = QModelIndex()) const;

            /**
             * Returns data from the item with the given \p index for the specified
             * \p role.
             * If the view requests an invalid index, an invalid variant is
             * returned.
             *
             * Reimplemented from QAbstractItemModel.
             *
             * \see QAbstractItemModel::data
             * \see Qt::ItemDataRole
             */
            QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

            /**
             * Reimplemented to show unavailable devices as disabled (but still
             * selectable).
             */
            Qt::ItemFlags flags(const QModelIndex &index) const;

            /**
             * Returns a list of indexes in the same order as they are in the
             * model. The indexes come from the ObjectDescription::index
             * method.
             *
             * This is useful to let the user define a list of preference.
             */
            QList<int> tupleIndexOrder() const;

            /**
             * Returns the ObjectDescription::index for the tuple
             * at the given position \p positionIndex. For example a
             * QComboBox will give you the currentIndex as the
             * position in the list. But to select the according
             * AudioOutputDevice using AudioOutputDevice::fromIndex
             * you can use this method.
             *
             * \param positionIndex The position in the list.
             */
            int tupleIndexAtPositionIndex(int positionIndex) const;

            /**
             * Returns the MIME data that dropMimeData() can use to create new
             * items.
             */
            QMimeData *mimeData(ObjectDescriptionType type, const QModelIndexList &indexes) const;

            /**
             * Moves the item at the given \p index up. In the resulting list
             * the items at index.row() and index.row() - 1 are swapped.
             *
             * Connected views are updated automatically.
             */
            void moveUp(const QModelIndex &index);

            /**
             * Moves the item at the given \p index down. In the resulting list
             * the items at index.row() and index.row() + 1 are swapped.
             *
             * Connected views are updated automatically.
             */
            void moveDown(const QModelIndex &index);

            void setModelData(const QList<QExplicitlySharedDataPointer<ObjectDescriptionData> > &data);
            QList<QExplicitlySharedDataPointer<ObjectDescriptionData> > modelData() const;
            QExplicitlySharedDataPointer<ObjectDescriptionData> modelData(const QModelIndex &index) const;
            Qt::DropActions supportedDropActions() const;
            bool dropMimeData(ObjectDescriptionType type, const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
            bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
            QStringList mimeTypes(ObjectDescriptionType type) const;

            ObjectDescriptionModelData(QAbstractListModel *);
        protected:
            ~ObjectDescriptionModelData();
            //ObjectDescriptionModelData(ObjectDescriptionModelDataPrivate *dd);
            ObjectDescriptionModelDataPrivate *const d;
    };

    /** \class ObjectDescriptionModel objectdescriptionmodel.h Phonon/ObjectDescriptionModel
     * \short The ObjectDescriptionModel class provides a model from
     * a list of ObjectDescription objects.
     *
     * ObjectDescriptionModel is a readonly model that supplies a list
     * using ObjectDescription::name() for the text and
     * ObjectDescription::description() for the tooltip. If set the properties
     * "icon" and "available" are used to set the decoration and disable the
     * item (disabled only visually, you can still select and drag it).
     *
     * It also provides the methods moveUp() and moveDown() to order the list.
     * Additionally drag and drop is possible so that
     * QAbstractItemView::InternalMove can be used.
     * The resulting order of the ObjectDescription::index() values can then be
     * retrieved using tupleIndexOrder().
     *
     * An example use case would be to give the user a QComboBox to select
     * the output device:
     * \code
     * QComboBox *cb = new QComboBox(parentWidget);
     * ObjectDescriptionModel *model = new ObjectDescriptionModel(cb);
     * model->setModelData(BackendCapabilities::availableAudioOutputDevices());
     * cb->setModel(model);
     * cb->setCurrentIndex(0); // select first entry
     * \endcode
     *
     * And to retrieve the selected AudioOutputDevice:
     * \code
     * int cbIndex = cb->currentIndex();
     * AudioOutputDevice selectedDevice = model->modelData(cbIndex);
     * \endcode
     *
     * \ingroup Frontend
     * \author Matthias Kretz <kretz@kde.org>
     */
    template<ObjectDescriptionType type>
    class ObjectDescriptionModel : public QAbstractListModel
    {
        public:
            Q_OBJECT_CHECK
            /** \internal */
            static PHONON_EXPORT const QMetaObject staticMetaObject;
            /** \internal */
            PHONON_EXPORT_ODM const QMetaObject *metaObject() const;
            /** \internal */
            PHONON_EXPORT_ODM void *qt_metacast(const char *_clname);
            //int qt_metacall(QMetaObject::Call _c, int _id, void **_a);

            /**
             * Returns the number of rows in the model. This value corresponds
             * to the size of the list passed through setModelData.
             *
             * \param parent The optional \p parent argument is used in most models to specify
             * the parent of the rows to be counted. Because this is a list if a
             * valid parent is specified the result will always be 0.
             *
             * Reimplemented from QAbstractItemModel.
             *
             * \see QAbstractItemModel::rowCount
             */
            inline int rowCount(const QModelIndex &parent = QModelIndex()) const { return d->rowCount(parent); } //krazy:exclude=inline

            /**
             * Returns data from the item with the given \p index for the specified
             * \p role.
             * If the view requests an invalid index, an invalid variant is
             * returned.
             *
             * Reimplemented from QAbstractItemModel.
             *
             * \see QAbstractItemModel::data
             * \see Qt::ItemDataRole
             */
            inline QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const { return d->data(index, role); } //krazy:exclude=inline

            /**
             * Reimplemented to show unavailable devices as disabled (but still
             * selectable).
             */
            inline Qt::ItemFlags flags(const QModelIndex &index) const { return d->flags(index); } //krazy:exclude=inline

            /**
             * Returns a list of indexes in the same order as they are in the
             * model. The indexes come from the ObjectDescription::index
             * method.
             *
             * This is useful to let the user define a list of preference.
             */
            inline QList<int> tupleIndexOrder() const { return d->tupleIndexOrder(); } //krazy:exclude=inline

            /**
             * Returns the ObjectDescription::index for the tuple
             * at the given position \p positionIndex. For example a
             * QComboBox will give you the currentIndex as the
             * position in the list. But to select the according
             * AudioOutputDevice using AudioOutputDevice::fromIndex
             * you can use this method.
             *
             * \param positionIndex The position in the list.
             */
            inline int tupleIndexAtPositionIndex(int positionIndex) const { return d->tupleIndexAtPositionIndex(positionIndex); } //krazy:exclude=inline

            /**
             * Returns the MIME data that dropMimeData() can use to create new
             * items.
             */
            inline QMimeData *mimeData(const QModelIndexList &indexes) const { return d->mimeData(type, indexes); } //krazy:exclude=inline

            /**
             * Moves the item at the given \p index up. In the resulting list
             * the items at index.row() and index.row() - 1 are swapped.
             *
             * Connected views are updated automatically.
             */
            inline void moveUp(const QModelIndex &index) { d->moveUp(index); } //krazy:exclude=inline

            /**
             * Moves the item at the given \p index down. In the resulting list
             * the items at index.row() and index.row() + 1 are swapped.
             *
             * Connected views are updated automatically.
             */
            inline void moveDown(const QModelIndex &index) { d->moveDown(index); } //krazy:exclude=inline

            /**
             * Constructs a ObjectDescription model with the
             * given \p parent.
             */
            explicit inline ObjectDescriptionModel(QObject *parent = 0) : QAbstractListModel(parent), d(new ObjectDescriptionModelData(this)) {} //krazy:exclude=inline

            /**
             * Constructs a ObjectDescription model with the
             * given \p parent and the given \p data.
             */
            explicit inline ObjectDescriptionModel(const QList<ObjectDescription<type> > &data, QObject *parent = 0) //krazy:exclude=inline
                : QAbstractListModel(parent), d(new ObjectDescriptionModelData(this)) { setModelData(data); }

            /**
             * Sets the model data using the list provided by \p data.
             *
             * All previous model data is cleared.
             */
            inline void setModelData(const QList<ObjectDescription<type> > &data) { //krazy:exclude=inline
                QList<QExplicitlySharedDataPointer<ObjectDescriptionData> > list;
                Q_FOREACH (const ObjectDescription<type> &desc, data) {
                    list << desc.d;
                }
                d->setModelData(list);
            }

            /**
             * Returns the model data.
             *
             * As the order of the list might have changed this can be different
             * to what was set using setModelData().
             */
            inline QList<ObjectDescription<type> > modelData() const { //krazy:exclude=inline
                QList<ObjectDescription<type> > ret;
                QList<QExplicitlySharedDataPointer<ObjectDescriptionData> > list = d->modelData();
                Q_FOREACH (const QExplicitlySharedDataPointer<ObjectDescriptionData> &data, list) {
                    ret << ObjectDescription<type>(data);
                }
                return ret;
            }

            /**
             * Returns one ObjectDescription of the model data for the given \p index.
             */
            inline ObjectDescription<type> modelData(const QModelIndex &index) const { return ObjectDescription<type>(d->modelData(index)); } //krazy:exclude=inline

            /**
             * This model supports drag and drop to copy or move
             * items.
             */
            inline Qt::DropActions supportedDropActions() const { return d->supportedDropActions(); } //krazy:exclude=inline

            /**
             * Accept drops from other models of the same ObjectDescriptionType.
             *
             * If a valid \p parent is given the dropped items will be inserted
             * above that item.
             */
            inline bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) { //krazy:exclude=inline
                return d->dropMimeData(type, data, action, row, column, parent);
            }

            /**
             * Removes count rows starting with the given row.
             *
             * If a valid \p parent is given no rows are removed since this is a
             * list model.
             *
             * Returns true if the rows were successfully removed; otherwise returns false.
             */
            inline bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) { //krazy:exclude=inline
                return d->removeRows(row, count, parent);
            }

            /**
             * Returns a list of supported drag and drop MIME types. Currently
             * it only supports one type used internally.
             */
            inline QStringList mimeTypes() const { return d->mimeTypes(type); } //krazy:exclude=inline

        protected:
            ObjectDescriptionModelData *const d;
    };

    typedef ObjectDescriptionModel<AudioOutputDeviceType> AudioOutputDeviceModel;
    typedef ObjectDescriptionModel<AudioCaptureDeviceType> AudioCaptureDeviceModel;
    typedef ObjectDescriptionModel<EffectType> EffectDescriptionModel;
    typedef ObjectDescriptionModel<AudioChannelType> AudioChannelDescriptionModel;
    typedef ObjectDescriptionModel<SubtitleType> SubtitleDescriptionModel;
/*
    typedef ObjectDescriptionModel<VideoOutputDeviceType> VideoOutputDeviceModel;
    typedef ObjectDescriptionModel<VideoCaptureDeviceType> VideoCaptureDeviceModel;
    typedef ObjectDescriptionModel<AudioCodecType> AudioCodecDescriptionModel;
    typedef ObjectDescriptionModel<VideoCodecType> VideoCodecDescriptionModel;
    typedef ObjectDescriptionModel<ContainerFormatType> ContainerFormatDescriptionModel;
    typedef ObjectDescriptionModel<VisualizationType> VisualizationDescriptionModel;*/

}

#endif //QT_NO_PHONON_OBJECTDESCRIPTIONMODEL

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_OBJECTDESCRIPTIONMODEL_H
// vim: sw=4 ts=4 tw=80
