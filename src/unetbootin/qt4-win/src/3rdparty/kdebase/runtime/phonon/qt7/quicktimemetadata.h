/*  This file is part of the KDE project.

    Copyright (C) 2007 Trolltech ASA. All rights reserved.

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

#ifndef Phonon_QT7_QUICKTIMEMETADATA_H
#define Phonon_QT7_QUICKTIMEMETADATA_H

#include <QuickTime/QuickTime.h>
#undef check // avoid name clash;

#include <phonon/mediasource.h>
#include <Carbon/Carbon.h>
#include <QtCore>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{
    class QuickTimeMetaData
    {
        public:
            QuickTimeMetaData();
            virtual ~QuickTimeMetaData();

            void setVideo(Movie movieRef);
            QMultiMap<QString, QString> metaData();
            Movie movieRef();

        private:
            Movie m_movieRef;
            QMultiMap<QString, QString> m_metaData;
            bool m_movieChanged;

            QString stripCopyRightSymbol(const QString &key);
            QString convertQuickTimeKeyToUserKey(const QString &key);
            OSStatus readMetaValue(QTMetaDataRef, QTMetaDataItem, QTPropertyClass, QTPropertyID, QTPropertyValuePtr *, ByteCount *);
            UInt32 getMetaType(QTMetaDataRef metaDataRef, QTMetaDataItem item);
            QString getMetaValue(QTMetaDataRef metaDataRef, QTMetaDataItem item, SInt32 id);
            void readFormattedData(QTMetaDataRef metaDataRef, OSType format, QMultiMap<QString, QString> &result);
            void readMetaData();
    };

}} // namespace Phonon::QT7

QT_END_NAMESPACE

#endif // Phonon_QT7_QUICKTIMEMETADATA_H
