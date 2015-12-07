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

#include "quicktimemetadata.h"
#include "quicktimevideoplayer.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{

QuickTimeMetaData::QuickTimeMetaData()
{
    m_videoPlayer = 0;
    m_movieChanged = false;
}

QuickTimeMetaData::~QuickTimeMetaData()
{
}

void QuickTimeMetaData::setVideo(QuickTimeVideoPlayer *videoPlayer)
{
    m_videoPlayer = videoPlayer;
    m_movieChanged = true;
    m_metaData.clear();
}

#ifdef QUICKTIME_C_API_AVAILABLE

QString QuickTimeMetaData::stripCopyRightSymbol(const QString &key)
{
    return key.right(key.length()-1);
}

QString QuickTimeMetaData::convertQuickTimeKeyToUserKey(const QString &key)
{
    if (key == QLatin1String("com.apple.quicktime.displayname"))
        return QLatin1String("nam");
    else if (key == QLatin1String("com.apple.quicktime.album"))
        return QLatin1String("alb");
    else if (key == QLatin1String("com.apple.quicktime.artist"))
        return QLatin1String("ART");
    else
        return QLatin1String("???");
}

OSStatus QuickTimeMetaData::readMetaValue(QTMetaDataRef metaDataRef, QTMetaDataItem item, QTPropertyClass propClass,
    QTPropertyID id, QTPropertyValuePtr *value, ByteCount *size)
{
	QTPropertyValueType type;
	ByteCount propSize;
	UInt32 propFlags;
	OSStatus err = QTMetaDataGetItemPropertyInfo(metaDataRef, item, propClass, id, &type, &propSize, &propFlags);
    BACKEND_ASSERT3(err == noErr, "Could not read meta data value size", NORMAL_ERROR, err)

    *value = malloc(propSize);

	err = QTMetaDataGetItemProperty(metaDataRef, item, propClass, id, propSize, *value, size);
    BACKEND_ASSERT3(err == noErr, "Could not read meta data value", NORMAL_ERROR, err)

    if (type == 'code' || type == 'itsk' || type == 'itlk') {
        // convert from native endian to big endian
    	OSTypePtr pType = (OSTypePtr)*value;
    	*pType = EndianU32_NtoB(*pType);
    }

	return err;
}

UInt32 QuickTimeMetaData::getMetaType(QTMetaDataRef metaDataRef, QTMetaDataItem item)
{
	QTPropertyValuePtr value = 0;
	ByteCount ignore = 0;
	OSStatus err = readMetaValue(
	    metaDataRef, item, kPropertyClass_MetaDataItem, kQTMetaDataItemPropertyID_DataType, &value, &ignore);
    BACKEND_ASSERT3(err == noErr, "Could not read meta data type", NORMAL_ERROR, 0)
    UInt32 type = *((UInt32 *) value);
    if (value)
	    free(value);
	return type;
}

QString QuickTimeMetaData::getMetaValue(QTMetaDataRef metaDataRef, QTMetaDataItem item, SInt32 id)
{
	QTPropertyValuePtr value = 0;
	ByteCount size = 0;
	OSStatus err = readMetaValue(metaDataRef, item, kPropertyClass_MetaDataItem, id, &value, &size);
    BACKEND_ASSERT3(err == noErr, "Could not read meta data item", NORMAL_ERROR, QString())
    BACKEND_ASSERT3(value != 0, "Could not read meta data item", NORMAL_ERROR, QString())

    QString string;
    UInt32 dataType = getMetaType(metaDataRef, item);
    switch (dataType){
    case kQTMetaDataTypeUTF8:
    case kQTMetaDataTypeMacEncodedText:
        string = PhononCFString::toQString(CFStringCreateWithBytes(0, (UInt8*)value, size, kCFStringEncodingUTF8, false));
        break;
    case kQTMetaDataTypeUTF16BE:
        string = PhononCFString::toQString(CFStringCreateWithBytes(0, (UInt8*)value, size, kCFStringEncodingUTF16BE, false));
        break;
    default:
        break;
    }

    if (value)
	    free(value);
    return string;
}

void QuickTimeMetaData::readFormattedData(QTMetaDataRef metaDataRef, OSType format, QMultiMap<QString, QString> &result)
{
	QTMetaDataItem item = kQTMetaDataItemUninitialized;
    OSStatus err = QTMetaDataGetNextItem(metaDataRef, format, item, kQTMetaDataKeyFormatWildcard, 0, 0, &item);
	while (err == noErr){
        QString key = getMetaValue(metaDataRef, item, kQTMetaDataItemPropertyID_Key);
        if (format == kQTMetaDataStorageFormatQuickTime)
            key = convertQuickTimeKeyToUserKey(key);
        else
            key = stripCopyRightSymbol(key);

	    if (!result.contains(key)){
	        QString val = getMetaValue(metaDataRef, item, kQTMetaDataItemPropertyID_Value);
            result.insert(key, val);
        }
        err = QTMetaDataGetNextItem(metaDataRef, format, item, kQTMetaDataKeyFormatWildcard, 0, 0, &item);
	}
}

#endif // QUICKTIME_C_API_AVAILABLE

void QuickTimeMetaData::readMetaData()
{
	if (!m_videoPlayer)
        return;
    QMultiMap<QString, QString> metaMap;
    
#ifdef QUICKTIME_C_API_AVAILABLE
	QTMetaDataRef metaDataRef;        
	OSStatus err = QTCopyMovieMetaData([m_videoPlayer->qtMovie() quickTimeMovie], &metaDataRef);
    BACKEND_ASSERT2(err == noErr, "Could not read QuickTime meta data", NORMAL_ERROR)

    readFormattedData(metaDataRef, kQTMetaDataStorageFormatUserData, metaMap);
    readFormattedData(metaDataRef, kQTMetaDataStorageFormatQuickTime, metaMap);
    readFormattedData(metaDataRef, kQTMetaDataStorageFormatiTunes, metaMap);
#else
	NSString *name = [m_videoPlayer->qtMovie() attributeForKey:@"QTMovieDisplayNameAttribute"];
	metaMap.insert(QLatin1String("nam"), QString::fromUtf8([name UTF8String]));
#endif // QUICKTIME_C_API_AVAILABLE

    m_metaData.insert(QLatin1String("ARTIST"), metaMap.value(QLatin1String("ART")));
    m_metaData.insert(QLatin1String("ALBUM"), metaMap.value(QLatin1String("alb")));
    m_metaData.insert(QLatin1String("TITLE"), metaMap.value(QLatin1String("nam")));
    m_metaData.insert(QLatin1String("DATE"), metaMap.value(QLatin1String("day")));
    m_metaData.insert(QLatin1String("GENRE"), metaMap.value(QLatin1String("gnre")));
    m_metaData.insert(QLatin1String("TRACKNUMBER"), metaMap.value(QLatin1String("trk")));
    m_metaData.insert(QLatin1String("DESCRIPTION"), metaMap.value(QLatin1String("des")));
}

QMultiMap<QString, QString> QuickTimeMetaData::metaData()
{
    if (m_videoPlayer && m_videoPlayer->hasMovie() && m_movieChanged)
        readMetaData();
    return m_metaData;
}

}}

QT_END_NAMESPACE
