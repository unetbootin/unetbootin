/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QEGL_P_H
#define QEGL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QGLWidget class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include "QtCore/qvarlengtharray.h"
#include "QtCore/qsize.h"
#include "QtGui/qimage.h"

#if defined(QT_OPENGL_ES) || defined(QT_OPENVG)

QT_BEGIN_INCLUDE_NAMESPACE
#if defined(QT_OPENGL_ES_2) || defined(QT_OPENVG)
#include <EGL/egl.h>
#else
#include <GLES/egl.h>
#endif
#if !defined(EGL_VERSION_1_3) && !defined(QEGL_NATIVE_TYPES_DEFINED)
#undef EGLNativeWindowType
#undef EGLNativePixmapType
#undef EGLNativeDisplayType
typedef NativeWindowType EGLNativeWindowType;
typedef NativePixmapType EGLNativePixmapType;
typedef NativeDisplayType EGLNativeDisplayType;
#define QEGL_NATIVE_TYPES_DEFINED 1
#endif
QT_END_INCLUDE_NAMESPACE

class QX11Info;
class QPaintDevice;
class QImage;
class QPixmap;
class QWidget;

QT_BEGIN_NAMESPACE

class Q_OPENGL_EXPORT QEglProperties
{
public:
    QEglProperties();
    QEglProperties(const QEglProperties& other) : props(other.props) {}
    ~QEglProperties() {}

    int value(int name) const;
    void setValue(int name, int value);
    bool removeValue(int name);

    const int *properties() const { return props.constData(); }

    void setPixelFormat(QImage::Format pixelFormat);
#ifdef Q_WS_X11
    void setVisualFormat(const QX11Info *xinfo);
#endif
    void setRenderableType(int api);

    bool reduceConfiguration();

    QString toString() const;

private:
    QVarLengthArray<int> props;
};

class Q_OPENGL_EXPORT QEglContext
{
public:
    QEglContext();
    ~QEglContext();

    enum API
    {
        OpenGL,
        OpenVG
    };

    enum PixelFormatMatch
    {
        ExactPixelFormat,
        BestPixelFormat
    };

    bool isValid() const;
    bool isSharing() const;

    void setApi(QEglContext::API api) { apiType = api; }
    bool openDisplay(QPaintDevice *device);
    bool chooseConfig(const QEglProperties& properties, PixelFormatMatch match = ExactPixelFormat);
    bool createContext(QEglContext *shareContext = 0);
    bool createSurface(QPaintDevice *device);
    bool recreateSurface(QPaintDevice *device);
    void setSurface(EGLSurface surface) { surf = surface; }

    void destroy();

    bool makeCurrent();
    bool doneCurrent();
    bool swapBuffers();

    void waitNative();
    void waitClient();

    QSize surfaceSize() const;

    bool configAttrib(int name, EGLint *value) const;

    void clearError() const { eglGetError(); }

    QEglContext::API api() const { return apiType; }

    EGLDisplay display() const { return dpy; }
    EGLContext context() const { return ctx; }
    EGLSurface surface() const { return surf; }
    EGLConfig config() const { return cfg; }

    QEglProperties configProperties(EGLConfig cfg = 0) const;

    static EGLDisplay defaultDisplay(QPaintDevice *device);
    static QString errorString(int code);

    void dumpAllConfigs();

private:
    QEglContext::API apiType;
    EGLDisplay dpy;
    EGLContext ctx;
    EGLSurface surf;
    EGLConfig cfg;
    bool share;
    void *reserved;     // For extension data in future versions.

    static EGLDisplay getDisplay(QPaintDevice *device);
};

QT_END_NAMESPACE

#endif // QT_OPENGL_ES || QT_OPENVG

#endif // QEGL_P_H
