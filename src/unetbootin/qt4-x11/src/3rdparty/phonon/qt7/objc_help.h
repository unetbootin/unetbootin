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

#ifndef Phonon_QT7_VIDEOWIDGET_OBJC_H
#define Phonon_QT7_VIDEOWIDGET_OBJC_H

#include <QuickTime/QuickTime.h>
#undef check // avoid name clash;

#include <AGL/agl.h>
#include <QtCore>
#include <phonon/effectparameter.h>

struct CiFilterInfo
{
    QList<QString> filterNames;
    QList<QString> filterDisplayNames;
};

struct CiFilterParameterInfo
{
    QList<QString> parameterNames;
    QList<Phonon::EffectParameter> parameters;
};

void *objc_createPool();
void objc_releasePool(void *pool);
int objc_getRetainCount(void *nsObject);

void *objc_createCiContext(AGLContext aglContext, AGLPixelFormat aglPixelFormat);
void objc_releaseCiContext(void *ciContext);

void objc_retainCiImage(void *ciImage);
void objc_releaseCiImage(void *ciImage);
void *objc_ciImageFromCvImageBuffer(void *cvImageBufferRef);
QSize objc_ciImageSize(void *ciImage);
void objc_drawCiImage(void *ciImage, QRect rect, void *ciContext);
void objc_drawCiImageInv(void *ciImage, QRect rect, void *ciContext);

void *objc_createCiFilter(int filterId);
void objc_releaseCiFilter(void *ciFilter);
void *objc_applyCiFilter(void *ciImage, void *ciFilter);
void objc_setCiFilterParameter(void *ciFilter, int parameterId, QVariant value);
QVariant objc_getCiFilterParameter(void *ciFilter, int parameterId);

CiFilterInfo *objc_getCiFilterInfo();
CiFilterParameterInfo objc_getCiFilterParameterInfo(void *ciFilter);

#endif // Phonon_QT7_VIDEOWIDGET_OBJC_H
