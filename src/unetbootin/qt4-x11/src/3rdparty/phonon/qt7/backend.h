/*  This file is part of the KDE project.

    Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).

    This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2.1 or 3 of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef Phonon_QT7_BACKEND_H
#define Phonon_QT7_BACKEND_H

#include <QtCore/QList>
#include <QtCore/QPointer>
#include <QtCore/QStringList>
#include <phonon/backendinterface.h>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{
    class Backend : public QObject, public BackendInterface
    {
        Q_OBJECT
        Q_INTERFACES(Phonon::BackendInterface)

        public:
            Backend();
            Backend(QObject *parent, const QStringList &args);
            virtual ~Backend();

            QObject *createObject(BackendInterface::Class, QObject *parent, const QList<QVariant> &args);
            QStringList availableMimeTypes() const;
            QList<int> objectDescriptionIndexes(ObjectDescriptionType type) const;
            QHash<QByteArray, QVariant> objectDescriptionProperties(ObjectDescriptionType type, int index) const;

            bool startConnectionChange(QSet<QObject *> nodes);
            bool connectNodes(QObject *source, QObject *sink);
            bool disconnectNodes(QObject *source, QObject *sink);
            bool endConnectionChange(QSet<QObject *> nodes);

        Q_SIGNALS:
            void objectDescriptionChanged(ObjectDescriptionType);

        private:
            bool quickTime7Available();
    };
}} // namespace Phonon::QT7

QT_END_NAMESPACE
#endif // Phonon_QT7_BACKEND_H
