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

#import <QTKit/QTKit.h>

#include <QByteArray>
#include <private/qt_mac_p.h>
#include <private/qcore_mac_p.h>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{

#if 0
static void dumpSupportedFileTypes()
{
    NSArray *fileTypes = [QTMovie movieFileTypes:QTIncludeCommonTypes];
    NSString *prefix = @"."; 
    for (unsigned int i=0; i<[fileTypes count]; ++i){
        NSString *type = [fileTypes objectAtIndex:i];
        type = [type stringByReplacingOccurrencesOfString:@"'" withString:@""];
        type = [type stringByReplacingOccurrencesOfString:@"." withString:@""];
        type = [prefix stringByAppendingString:type];
        NSLog(type);
    }    
}
#endif

Movie QuickTimeStreamReader_convertToQuickTime(void *movie)
{
    return [(QTMovie *)movie quickTimeMovie];
}

void QuickTimeStreamReader_release(void *movie)
{
    [(QTMovie *)movie release];
}

void *QuickTimeStreamReader_createMovieFromBuffer(QByteArray &buffer, NSString *fileType)
{    
    QMacCocoaAutoReleasePool pool;    
    NSData *data = [NSData dataWithBytesNoCopy:buffer.data() length:buffer.size() freeWhenDone:NO];
    QTDataReference *dataRef = [QTDataReference dataReferenceWithReferenceToData:data name:fileType MIMEType:@""];
    NSDictionary *props = [NSDictionary dictionaryWithObjectsAndKeys:
                dataRef, QTMovieDataReferenceAttribute,
                [NSNumber numberWithBool:YES], QTMovieOpenAsyncOKAttribute,
                [NSNumber numberWithBool:YES], QTMovieIsActiveAttribute,
                [NSNumber numberWithBool:YES], QTMovieResolveDataRefsAttribute,
                nil];

    QTMovie *movie = [QTMovie movieWithAttributes:props error:0];
    if (movie)
        [movie retain];
    return movie;
}

void *QuickTimeStreamReader_createMovieFromBufferGuessType(QByteArray &buffer)
{
    QMacCocoaAutoReleasePool pool;    
    void *movie = 0;

    // It turns out to be better to just try the standard file types rather
    // than using e.g [QTMovie movieFileTypes:QTIncludeCommonTypes]. Some
    // codes thinks they can decode the stream, and crash...
#define PhononTryType(type) ClearMoviesStickyError(); \
        if ((movie = QuickTimeStreamReader_createMovieFromBuffer(buffer, @"."type))) return movie;
    PhononTryType("avi");
    PhononTryType("mp4");
    PhononTryType("m4p");
    PhononTryType("m1s");
    PhononTryType("mp3");
    PhononTryType("mpeg");
    PhononTryType("mov");
    PhononTryType("ogg");
    PhononTryType("wav");
    PhononTryType("wmv");
#undef PhononTryType(type)
    return 0;
}

}} //namespace Phonon::QT7

QT_END_NAMESPACE

