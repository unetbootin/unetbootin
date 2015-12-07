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

#include "objc_help.h"
#include "backendheader.h"
#import <QuartzCore/QuartzCore.h>
#include "quicktimevideoplayer.h"

void *objc_createPool()
{
    return [[NSAutoreleasePool alloc] init];
}

void objc_releasePool(void *pool)
{
    [(NSAutoreleasePool *) pool release];
}

int objc_getRetainCount(void *nsObject)
{
    return (int)[(NSObject *) nsObject retainCount];
}

///////////////////////////////////////////////////////////////////////////////////////////////

void *objc_createCiContext(AGLContext aglContext, AGLPixelFormat aglPixelFormat)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    CGLContextObj cglContext = 0;
    BACKEND_ASSERT3(aglGetCGLContext(aglContext, (void **)&cglContext),
        "Could not get cgl context from agl context", FATAL_ERROR, 0)
        
    CGLPixelFormatObj cglPixelFormat = 0;
    BACKEND_ASSERT3(aglGetCGLPixelFormat(aglPixelFormat, (void **)&cglPixelFormat),
        "Could not get cgl pixel format from agl pixel format", FATAL_ERROR, 0)

    CIContext *ciContext = [[CIContext contextWithCGLContext:cglContext pixelFormat:cglPixelFormat options:nil] retain];
    [pool release];
    return ciContext;
}

void objc_releaseCiContext(void *ciContext)
{
    [(CIContext *)ciContext release];
}

///////////////////////////////////////////////////////////////////////////////////////////////

void objc_retainCiImage(void *ciImage)
{
    [(CIImage *) ciImage retain];
}

void objc_releaseCiImage(void *ciImage)
{
    [(CIImage *) ciImage release];
}

QSize objc_imageSize(void *ciImage)
{
    CGRect r = [(CIImage *) ciImage extent];
    return QSize(r.size.width, r.size.height);
}        

void *objc_ciImageFromCvImageBuffer(void *cvImageBufferRef)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    void *image = (void *)[[CIImage imageWithCVImageBuffer:(CVImageBufferRef) cvImageBufferRef] retain];
//    return [CIImage imageWithCVImageBuffer:(CVImageBufferRef) cvImageBufferRef];
    [pool release];
    return image;
}

void objc_drawCiImage(void *ciImage, QRect rect, void *ciContext)
{        
    [(CIContext *) ciContext drawImage:(CIImage *) ciImage
        inRect:CGRectMake(rect.x(), rect.y(), rect.width(), rect.height())
        fromRect:[(CIImage *) ciImage extent]];
    glFlush();
}

void objc_drawCiImageInv(void *ciImage, QRect rect, void *ciContext)
{        
    [(CIContext *) ciContext drawImage:(CIImage *) ciImage
        inRect:CGRectMake(rect.x(), rect.y() + rect.height(), rect.width(), -rect.height())
        fromRect:[(CIImage *) ciImage extent]];
    glFlush();
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool acceptCiFilter(CIFilter *filter)
{
    // For now, only accept filters that has not other attributes
    // than NSNumbers.
    bool accepted = true;
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init]; 
    NSDictionary *filterDictionary = [filter attributes];
    NSArray *parameters = [filterDictionary allKeys];
    
    // Iterate through all parameters of the filter:
    for (uint parameterId=0; parameterId<[parameters count]; parameterId++){
        NSString *parameterName = (NSString*) [parameters objectAtIndex:parameterId];
        if ([parameterName isEqualToString:@"inputImage"]
            || [parameterName isEqualToString:@"outputImage"])
            continue;
            
        NSObject *parameterValue = [filterDictionary valueForKey:parameterName];
        if (parameterValue){
            if ([parameterValue isKindOfClass:[NSDictionary class]]){
                NSDictionary *parameterDictionary = (NSDictionary *) parameterValue;
                NSString *attributeClass = [parameterDictionary valueForKey:@"CIAttributeClass"];
                if (attributeClass && ![attributeClass isEqualToString:@"NSNumber"]){
                    accepted = false;
                    break;
                }   
            }
        }
    }
    
    [pool release];
    return accepted;
}

CiFilterInfo *ciFilterInfo = 0; // TODO: Find some way to delete this cache?
CiFilterInfo *objc_getCiFilterInfo()
{
    if (!ciFilterInfo){
        ciFilterInfo = new CiFilterInfo;
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];    
        NSArray *nslist = [CIFilter filterNamesInCategories:nil];
        for (uint i=0; i<[nslist count]; i++){
            NSString *filterName = (NSString*) [nslist objectAtIndex:i];
            CIFilter *filter = [CIFilter filterWithName:filterName];
            if (acceptCiFilter(filter)){
                NSDictionary *filterDictionary = [filter attributes];
                NSString *filterDispName = [filterDictionary valueForKey:@"CIAttributeFilterDisplayName"];
                if (!filterDispName)
                    filterDispName = filterName;
                ciFilterInfo->filterNames.append([filterName UTF8String]);
                ciFilterInfo->filterDisplayNames.append([filterDispName UTF8String]);
            }
        }
        [pool release];
    }
    return ciFilterInfo;
}

void *objc_createCiFilter(int filterId)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];    
    CiFilterInfo *info = objc_getCiFilterInfo();
    NSString *filterName = (NSString *) PhononCFString::toCFStringRef(info->filterNames[filterId]);
    CIFilter *filter = [CIFilter filterWithName:filterName];
    if (filter){
        [filter retain];
        [filter setDefaults];
    }
    [pool release];
    return filter;
}

void objc_releaseCiFilter(void *ciFilter)
{
    [(CIFilter *) ciFilter release];
}

void objc_setCiFilterParameter(void *ciFilter, int parameterId, QVariant value)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    QString name = objc_getCiFilterParameterInfo(ciFilter).parameterNames[parameterId];
    NSString *nsName = (NSString *) PhononCFString::toCFStringRef(name);
    [(CIFilter *) ciFilter setValue:[NSNumber numberWithFloat:value.toDouble()] forKey:nsName];
    [pool release];
}

QVariant objc_getCiFilterParameter(void *ciFilter, int parameterId)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    QString name = objc_getCiFilterParameterInfo(ciFilter).parameterNames[parameterId];
    NSString *nsName = (NSString *) PhononCFString::toCFStringRef(name);
    NSNumber *value = [(CIFilter *) ciFilter valueForKey:nsName];
    QVariant ret([value doubleValue]);
    [pool release];
    return ret;
}

void *objc_applyCiFilter(void *ciImage, void* ciFilter)
{
    CIImage *inputImage = (CIImage *) ciImage;    
    CIFilter *filter = (CIFilter *) ciFilter;
    [filter setValue:inputImage forKey:@"inputImage"];
    return [filter valueForKey:@"outputImage"];
}

CiFilterParameterInfo objc_getCiFilterParameterInfo(void *ciFilter)
{
    CiFilterParameterInfo info;
    
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init]; 
    CIFilter *filter = (CIFilter *) ciFilter;
    NSDictionary *filterDictionary = [filter attributes];
    NSArray *parameters = [filterDictionary allKeys];
    
    // Iterate through all parameters of the filter:
    for (uint parameterId=0; parameterId<[parameters count]; parameterId++){
        NSString *parameterName = (NSString*) [parameters objectAtIndex:parameterId];
        NSObject *parameterValue = [filterDictionary valueForKey:parameterName];
        if (parameterValue){
            if ([parameterValue isKindOfClass:[NSDictionary class]]){
                NSDictionary *parameterDictionary = (NSDictionary *) parameterValue;
                NSString *attributeClass = [parameterDictionary valueForKey:@"CIAttributeClass"];
                if (attributeClass && [attributeClass isEqualToString:@"NSNumber"]){
                    // Only parameters that stores an NSNumber are interresting.
                    NSString *parameterDispName = [parameterDictionary valueForKey:@"CIAttributeDisplayName"];
                    NSNumber *min = [parameterDictionary valueForKey:@"CIAttributeSliderMin"];
                    NSNumber *max = [parameterDictionary valueForKey:@"CIAttributeSliderMax"];
                    NSNumber *def = [parameterDictionary valueForKey:@"CIAttributeDefault"];
                    if (!parameterDispName)
                        parameterDispName = parameterName;
                    
                    // Add the parameter to the list:
                    info.parameterNames.append([parameterName UTF8String]);
                    info.parameters.append(Phonon::EffectParameter(parameterId, [parameterDispName UTF8String], 0,
                            QVariant(def ? [def doubleValue] : 0),
                            QVariant(min ? [min doubleValue] : 0),
                            QVariant(max ? [max doubleValue] : 0)));
                }   
            }
        }
    }
    
    [pool release];    
    return info;
}




