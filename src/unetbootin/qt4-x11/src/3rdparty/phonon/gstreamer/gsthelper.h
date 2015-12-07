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

#ifndef Phonon_GSTREAMER_GSTHELPER_H
#define Phonon_GSTREAMER_GSTHELPER_H

#include "common.h"

#include <gst/gst.h>

QT_BEGIN_NAMESPACE

template<class T> class QList;
class QByteArray;

namespace Phonon
{
namespace Gstreamer
{
class GstHelper
{
public:
    static QList<QByteArray> extractProperties(GstElement *elem, const QByteArray &value);
    static bool setProperty(GstElement *elem, const char *propertyName, const QByteArray &propertyValue);
    static QByteArray property(GstElement *elem, const char *propertyName);
    static QByteArray name(GstObject *elem);
    static GstElement* createPluggablePlaybin();
};

} // ns Gstreamer
} // ns Phonon

QT_END_NAMESPACE

#endif // Phonon_GSTREAMER_GSTHELPER_H
