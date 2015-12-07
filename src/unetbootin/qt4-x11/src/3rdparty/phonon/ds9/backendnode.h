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

#ifndef PHONON_BACKENDNODE_H
#define PHONON_BACKENDNODE_H

#include "phononds9_namespace.h"

#include <QtCore/QObject>
#include <QtCore/QVector>

#include "compointer.h"

QT_BEGIN_NAMESPACE



namespace Phonon
{
    namespace DS9
    {
        class MediaObject;
        typedef ComPointer<IPin> InputPin;
        typedef ComPointer<IPin> OutputPin;
        typedef ComPointer<IBaseFilter> Filter;
        typedef ComPointer<IGraphBuilder> Graph;

        class BackendNode : public QObject
        {
            Q_OBJECT

        public:
            BackendNode(QObject *parent);
            virtual ~BackendNode();

            MediaObject *mediaObject() const {return m_mediaObject;}

            static QList<InputPin> pins(const Filter &, PIN_DIRECTION);

            Filter filter(int index) const { return m_filters[index]; }
            //add a pointer to the base Media Object (giving access to the graph and error management)
            void setMediaObject(MediaObject *mo);

            //called by the connections to tell the node that it's been connection to anothe one through its 'inpin' input port
            virtual void connected(BackendNode *, const InputPin& inpin) {}

            private Q_SLOTS:
                void mediaObjectDestroyed();

        protected:
            Filter m_filters[FILTER_COUNT];
            MediaObject *m_mediaObject;
        };
    }
}

QT_END_NAMESPACE

#endif
