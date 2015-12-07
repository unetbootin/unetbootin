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

#include "backendinfo.h"
#include "backendheader.h"
#include <private/qcore_mac_p.h>

#include <AudioToolbox/AudioToolbox.h>
#include <AudioUnit/AudioUnit.h>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{

QString BackendInfo::quickTimeVersionString()
{
	long version;
    OSStatus err = Gestalt(gestaltQuickTimeVersion, &version);
    if (err != noErr)
        return QString("00.0.0.0000");
    QString versionString = QString("%1%2.%3.%4.%5%6%7%8")
        .arg((version >> (7*4)) & 0xF)
        .arg((version >> (6*4)) & 0xF)
        .arg((version >> (5*4)) & 0xF)
        .arg((version >> (4*4)) & 0xF)
        .arg((version >> (3*4)) & 0xF)
        .arg((version >> (2*4)) & 0xF)
        .arg((version >> (1*4)) & 0xF)
        .arg((version >> (0*4)) & 0xF);
    return versionString;
}

bool BackendInfo::isQuickTimeVersionAvailable(int minHexVersion)
{
    // minHexVersion == 0x0741 means version 7.4.1
	long qtHexVersion;
    OSStatus err = Gestalt(gestaltQuickTimeVersion, &qtHexVersion);
    return (err == noErr) ? ((qtHexVersion >> 16) >= minHexVersion) : 0;
}

QMap<QString, QString> BackendInfo::quickTimeMimeTypes(Scope scope)
{
    QMap<QString, QString> mimeTypes;
    ARGUMENT_UNSUPPORTED(scope, Out, NORMAL_ERROR, mimeTypes)

    ComponentDescription description;
    description.componentType = MovieImportType;
    description.componentSubType = 0;
    description.componentManufacturer = 0;
    description.componentFlags = hasMovieImportMIMEList | canMovieImportFiles;
    description.componentFlagsMask = canMovieImportFiles | movieImportSubTypeIsFileExtension | hasMovieImportMIMEList;
    Component component = FindNextComponent(0, &description);

    while (component) {
        QTAtomContainer mimeList = 0;
        OSStatus err = MovieImportGetMIMETypeList((MovieImportComponent)component, &mimeList);
        if (err == noErr){
            int count = QTCountChildrenOfType(mimeList, kParentAtomIsContainer, 0);
            for (int i=1; i<=count; ++i){
                QString mimeType = getMimeTypeTag(mimeList, i, kMimeInfoMimeTypeTag);
                if (mimeType.startsWith(QLatin1String("audio")) || mimeType.startsWith(QLatin1String("video"))){
                    QString mimeDescription = getMimeTypeTag(mimeList, i, kMimeInfoDescriptionTag);
                    if (err == noErr && !mimeType.isEmpty())
                        mimeTypes.insert(mimeType, mimeDescription);
                }
            }
         }
         QTDisposeAtomContainer(mimeList);
         component = FindNextComponent(component, &description);
    }

    return mimeTypes;
}

QString BackendInfo::getMimeTypeTag(QTAtomContainer mimeList, int index, OSType type)
{
    QTAtom mimeAtom = QTFindChildByIndex(mimeList, kParentAtomIsContainer, type, index, 0);
    char mimeCharArray[256];
    long length;
    OSStatus err = QTCopyAtomDataToPtr(mimeList, mimeAtom, true, sizeof(mimeCharArray)-1, mimeCharArray, &length);
    if (err == noErr)
        return QString::fromAscii(mimeCharArray, length);
    return QString();
}

QStringList BackendInfo::quickTimeCompressionFormats()
{
    QStringList result;

    ComponentInstance component = 0;
    OSStatus err = OpenADefaultComponent(StandardCompressionType, StandardCompressionSubTypeAudio, &component);
    BACKEND_ASSERT3(err == noErr, "Could not open component for retrieving awailable compression formats", NORMAL_ERROR, result)

    UInt32 size;
    err = QTGetComponentPropertyInfo(component, kQTPropertyClass_SCAudio, kQTSCAudioPropertyID_AvailableCompressionFormatNamesList, 0, &size,0);
    BACKEND_ASSERT3(err == noErr, "Could not get awailable compression formats", NORMAL_ERROR, result)

    CFArrayRef formats[size];
    err = QTGetComponentProperty(component, kQTPropertyClass_SCAudio, kQTSCAudioPropertyID_AvailableCompressionFormatNamesList, size, &formats, &size);
    BACKEND_ASSERT3(err == noErr, "Could not get awailable compression formats", NORMAL_ERROR, result)

    CFIndex count = CFArrayGetCount(*formats);
    for (CFIndex i=0; i<count; ++i){
        const CFStringRef name = (const struct __CFString *) CFArrayGetValueAtIndex(*formats, i);
        result << QCFString::toQString(name);
    }
    return result;
}


QStringList BackendInfo::coreAudioCodecs(Scope scope)
{
    QStringList result;
    UInt32 size;
    OSStatus err;
    OSType *formatIDs;

    OSType encodersOrDecoders = (scope == In)
        ? kAudioFormatProperty_EncodeFormatIDs : kAudioFormatProperty_DecodeFormatIDs;

    err = AudioFormatGetPropertyInfo(encodersOrDecoders, 0, NULL,  &size);
    BACKEND_ASSERT3(err == noErr, "Could not get awailable decoders/encoders", NORMAL_ERROR, result)

    formatIDs = (OSType*)malloc(size);
    UInt32 numFormats = size / sizeof(OSType);
    err = AudioFormatGetProperty(encodersOrDecoders, 0, NULL, &size,  formatIDs);
    BACKEND_ASSERT(err == noErr, "Could not get awailable decoders/encoders", NORMAL_ERROR){
        free(formatIDs);
        return result;
    }

    for (UInt32 i=0; i<numFormats; ++i){
        AudioStreamBasicDescription absd;
        memset(&absd, 0, sizeof(absd));
        absd.mFormatID = formatIDs[i];

        CFStringRef name;
        size = sizeof(CFStringRef);
        err = AudioFormatGetProperty(kAudioFormatProperty_FormatName, sizeof(absd), &absd,  &size, &name);
        BACKEND_ASSERT(err == noErr, "Could not get awailable decoder/encoder names", NORMAL_ERROR){
            free(formatIDs);
            return result;
        }
        result << QCFString::toQString(name);
    }
    free(formatIDs);
    return result;
}

QStringList BackendInfo::coreAudioFileTypes(Scope scope)
{
    QStringList result;
    OSStatus err;
    UInt32 propertySize;

    OSType readOrwrite = (scope == In)
        ? kAudioFileGlobalInfo_ReadableTypes : kAudioFileGlobalInfo_WritableTypes;

    err = AudioFileGetGlobalInfoSize(readOrwrite, 0, NULL, &propertySize);
    BACKEND_ASSERT3(err == noErr, "Could not get core audio file types", NORMAL_ERROR, result)

    OSType *types = (OSType*)malloc(propertySize);
    err = AudioFileGetGlobalInfo(readOrwrite, 0, NULL, &propertySize,  types);
    BACKEND_ASSERT3(err == noErr, "Could not get core audio file types", NORMAL_ERROR, result)

    UInt32 numTypes = propertySize / sizeof(OSType);
    for (UInt32 i=0; i<numTypes; ++i){
        CFStringRef name;
        UInt32 outSize = sizeof(name);
        err = AudioFileGetGlobalInfo(kAudioFileGlobalInfo_FileTypeName, sizeof(OSType), types+i, &outSize, &name);
        BACKEND_ASSERT3(err == noErr, "Could not get core audio file type names", NORMAL_ERROR, result)
        result << QCFString::toQString(name);
    }
    return result;
}

}}

QT_END_NAMESPACE

