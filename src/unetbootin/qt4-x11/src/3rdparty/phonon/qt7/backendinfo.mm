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

#include "backendinfo.h"
#include "backendheader.h"

#include <AudioToolbox/AudioToolbox.h>
#include <AudioUnit/AudioUnit.h>
#include <CoreServices/CoreServices.h>

#import <QTKit/QTMovie.h>

#ifdef QUICKTIME_C_API_AVAILABLE
    #include <QuickTime/QuickTime.h>
    #undef check // avoid name clash;
#endif

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{

QString BackendInfo::quickTimeVersionString()
{
	SInt32 version;
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
	SInt32 qtHexVersion;
    OSStatus err = Gestalt(gestaltQuickTimeVersion, &qtHexVersion);
    return (err == noErr) ? ((qtHexVersion >> 16) >= minHexVersion) : 0;
}

#ifdef QUICKTIME_C_API_AVAILABLE
static QString getMimeTypeTag(QTAtomContainer mimeList, int index, OSType type)
{
    QTAtom mimeAtom = QTFindChildByIndex(mimeList, kParentAtomIsContainer, type, index, 0);
    char mimeCharArray[256];
    long length;
    OSStatus err = QTCopyAtomDataToPtr(mimeList, mimeAtom, true, sizeof(mimeCharArray)-1, mimeCharArray, &length);
    if (err == noErr)
        return QString::fromAscii(mimeCharArray, length);
    return QString();
}
#endif // QUICKTIME_C_API_AVAILABLE

#ifdef QUICKTIME_C_API_AVAILABLE
QStringList BackendInfo::quickTimeMimeTypes(Scope scope)
{
    QStringList mimeTypes;
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
                    if (err == noErr && !mimeType.isEmpty())
                        mimeTypes << mimeType;
                }
            }
         }
         QTDisposeAtomContainer(mimeList);
         component = FindNextComponent(component, &description);
    }
	mimeTypes.sort();
    return mimeTypes;
}

#else // QUICKTIME_C_API_AVAILABLE == false

QString mimeForExtensionAudio(const QString &ext)
{
	if (ext == "3g2") return QLatin1String("audio/3g2"); 
	if (ext == "3gp") return QLatin1String("audio/3gp"); 
	if (ext == "aac") return QLatin1String("audio/aac"); 
	if (ext == "ac3") return QLatin1String("audio/ac3"); 
	if (ext == "aif") return QLatin1String("audio/aif"); 
	if (ext == "aifc") return QLatin1String("audio/aifc"); 
	if (ext == "aiff") return QLatin1String("audio/aiff"); 
	if (ext == "amr") return QLatin1String("audio/amr"); 
	if (ext == "au") return QLatin1String("audio/au");
	if (ext == "bwf") return QLatin1String("audio/bwf");
	if (ext == "caf") return QLatin1String("audio/caf");
	if (ext == "cdda") return QLatin1String("audio/cdda");
	if (ext == "gsm") return QLatin1String("audio/gsm");
	if (ext == "kar") return QLatin1String("audio/kar");
	if (ext == "m1a") return QLatin1String("audio/m1a");
	if (ext == "m1s") return QLatin1String("audio/m1s");
	if (ext == "m3u") return QLatin1String("audio/m3u");
	if (ext == "m3url") return QLatin1String("audio/m3url"); 
	if (ext == "mid") return QLatin1String("audio/mid");
	if (ext == "midi") return QLatin1String("audio/midi");
	if (ext == "mka") return QLatin1String("audio/mka");
	if (ext == "mp3") return QLatin1String("audio/mp3");
	if (ext == "mp4") return QLatin1String("audio/mp4");
	if (ext == "mpa") return QLatin1String("audio/mpa");
	if (ext == "mpeg") return QLatin1String("audio/mpeg");
	if (ext == "mpg") return QLatin1String("audio/mpg");
	if (ext == "mpg4") return QLatin1String("audio/mpg4");
	if (ext == "mpm") return QLatin1String("audio/mpm"); 
	if (ext == "qcp") return QLatin1String("audio/qcp"); 
	if (ext == "sd2") return QLatin1String("audio/sd2"); 
	if (ext == "smf") return QLatin1String("audio/smf"); 
	if (ext == "snd") return QLatin1String("audio/snd"); 
	if (ext == "ulw") return QLatin1String("audio/ulw"); 
	if (ext == "wav") return QLatin1String("audio/wav"); 
	if (ext == "wax") return QLatin1String("audio/wax"); 
	if (ext == "wma") return QLatin1String("audio/wma"); 
	return QString();
}

QString mimeForExtensionVideo(const QString &ext)
{
	if (ext == "3g2") return QLatin1String("video/3g2"); 
	if (ext == "3gp") return QLatin1String("video/3gp"); 
	if (ext == "asf") return QLatin1String("video/asf"); 
	if (ext == "asx") return QLatin1String("video/asx"); 
	if (ext == "avi") return QLatin1String("video/avi"); 
	if (ext == "dif") return QLatin1String("video/dif"); 
	if (ext == "dv") return QLatin1String("video/dv"); 
	if (ext == "flc") return QLatin1String("video/flc"); 
	if (ext == "fli") return QLatin1String("video/fli"); 
	if (ext == "m15") return QLatin1String("video/m15"); 
	if (ext == "m1a") return QLatin1String("video/m1a"); 
	if (ext == "m1s") return QLatin1String("video/m1s"); 
	if (ext == "m1v") return QLatin1String("video/m1v"); 
	if (ext == "m75") return QLatin1String("video/m75"); 
	if (ext == "mkv") return QLatin1String("video/mkv"); 
	if (ext == "mp4") return QLatin1String("video/mp4"); 
	if (ext == "mpa") return QLatin1String("video/mpa"); 
	if (ext == "mpeg") return QLatin1String("video/mpeg"); 
	if (ext == "mpg") return QLatin1String("video/mpg"); 
	if (ext == "mpg4") return QLatin1String("video/mpg4"); 
	if (ext == "mpm") return QLatin1String("video/mpm"); 
	if (ext == "mpv") return QLatin1String("video/mpv"); 
	if (ext == "vfw") return QLatin1String("video/vfw"); 
	if (ext == "wm") return QLatin1String("video/wm"); 
	if (ext == "wmv") return QLatin1String("video/wmv"); 
	if (ext == "wmx") return QLatin1String("video/wmx"); 
	if (ext == "wvx") return QLatin1String("video/wvx");
	return QString();
}

QStringList BackendInfo::quickTimeMimeTypes(Scope scope)
{
    QStringList mimeTypes;
    QStringList fileExtensions;
    ARGUMENT_UNSUPPORTED(scope, Out, NORMAL_ERROR, mimeTypes)

	PhononAutoReleasePool pool;
	NSArray *fileTypes = [QTMovie movieFileTypes:QTIncludeAllTypes];
	for (NSString *type in fileTypes){
		QString formattedType = QString::fromUtf8([type UTF8String]);
		formattedType = formattedType.remove('\'').remove('.').toLower();
		QString audioMime = mimeForExtensionAudio(formattedType);
		QString videoMime = mimeForExtensionVideo(formattedType);
		if (!audioMime.isEmpty())
			mimeTypes << audioMime;
		if (!videoMime.isEmpty())
			mimeTypes << videoMime;
		if (audioMime.isEmpty() && videoMime.isEmpty())
			fileExtensions << QLatin1String("application/x-qt-") + formattedType;
	}
	mimeTypes.sort();
	fileExtensions.sort();
    return mimeTypes + fileExtensions;
}
#endif // QUICKTIME_C_API_AVAILABLE

QStringList BackendInfo::quickTimeCompressionFormats()
{
    QStringList result;

#ifdef QUICKTIME_C_API_AVAILABLE

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
        result << PhononCFString::toQString(name);
    }
    
#endif // QUICKTIME_C_API_AVAILABLE
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
        result << PhononCFString::toQString(name);
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
        result << PhononCFString::toQString(name);
    }
    return result;
}

}}

QT_END_NAMESPACE

