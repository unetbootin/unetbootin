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

#include <QtGui/qpaintdevice.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qwidget.h>
#include <QtCore/qdebug.h>
#include "qegl_p.h"

#if defined(QT_OPENGL_ES) || defined(QT_OPENVG)

QT_BEGIN_NAMESPACE

QEglContext::QEglContext()
{
    apiType = OpenGL;
    dpy = EGL_NO_DISPLAY;
    ctx = EGL_NO_CONTEXT;
    surf = EGL_NO_SURFACE;
    cfg = 0;
    share = false;
    reserved = 0;
}

QEglContext::~QEglContext()
{
    destroy();
}

bool QEglContext::isValid() const
{
    return (ctx != EGL_NO_CONTEXT);
}

bool QEglContext::isSharing() const
{
    return share;
}

// Open the EGL display associated with "device".
bool QEglContext::openDisplay(QPaintDevice *device)
{
    if (dpy == EGL_NO_DISPLAY)
        dpy = defaultDisplay(device);
    return (dpy != EGL_NO_DISPLAY);
}

// Choose a configuration that matches "properties".
bool QEglContext::chooseConfig
        (const QEglProperties& properties, PixelFormatMatch match)
{
    QEglProperties props(properties);
    EGLConfig *configs;
    EGLint matching, size;
    do {
        // Get the number of matching configurations for this set of properties.
        matching = 0;
        if (!eglChooseConfig(dpy, props.properties(), 0, 256, &matching) || !matching)
            continue;

        // If we want the best pixel format, then return the first
        // matching configuration.
        if (match == BestPixelFormat) {
            eglChooseConfig(dpy, props.properties(), &cfg, 1, &matching);
            if (matching < 1)
                continue;
            return true;
        }

        // Fetch all of the matching configurations and find the
        // first that matches the pixel format we wanted.
        size = matching;
        configs = new EGLConfig [size];
        eglChooseConfig(dpy, props.properties(), configs, size, &matching);
        for (EGLint index = 0; index < size; ++index) {
            EGLint red, green, blue, alpha;
            eglGetConfigAttrib(dpy, configs[index], EGL_RED_SIZE, &red);
            eglGetConfigAttrib(dpy, configs[index], EGL_GREEN_SIZE, &green);
            eglGetConfigAttrib(dpy, configs[index], EGL_BLUE_SIZE, &blue);
            eglGetConfigAttrib(dpy, configs[index], EGL_ALPHA_SIZE, &alpha);
            if (red == props.value(EGL_RED_SIZE) &&
                    green == props.value(EGL_GREEN_SIZE) &&
                    blue == props.value(EGL_BLUE_SIZE) &&
                    (props.value(EGL_ALPHA_SIZE) == EGL_DONT_CARE ||
                     alpha == props.value(EGL_ALPHA_SIZE))) {
                cfg = configs[index];
                delete [] configs;
                return true;
            }
        }
        delete [] configs;
    } while (props.reduceConfiguration());

#ifdef EGL_BIND_TO_TEXTURE_RGBA
    // Don't report an error just yet if we failed to get a pbuffer
    // configuration with texture rendering.  Only report failure if
    // we cannot get any pbuffer configurations at all.
    if (props.value(EGL_BIND_TO_TEXTURE_RGBA) == EGL_DONT_CARE &&
        props.value(EGL_BIND_TO_TEXTURE_RGB) == EGL_DONT_CARE)
#endif
    {
        qWarning() << "QEglContext::chooseConfig(): Could not find a suitable EGL configuration";
        qWarning() << "Requested:" << props.toString();
        qWarning() << "Available:";
        dumpAllConfigs();
    }
    return false;
}

// Create the EGLContext.
bool QEglContext::createContext(QEglContext *shareContext)
{
    // We need to select the correct API before calling eglCreateContext().
#ifdef EGL_OPENGL_ES_API
    if (apiType == OpenGL)
        eglBindAPI(EGL_OPENGL_ES_API);
#endif
#ifdef EGL_OPENVG_API
    if (apiType == OpenVG)
        eglBindAPI(EGL_OPENVG_API);
#endif

    // Create a new context for the configuration.
    QEglProperties contextProps;
#if defined(QT_OPENGL_ES_2)
    if (apiType == OpenGL)
        contextProps.setValue(EGL_CONTEXT_CLIENT_VERSION, 2);
#endif
    if (shareContext && shareContext->ctx == EGL_NO_CONTEXT)
        shareContext = 0;
    if (shareContext) {
        ctx = eglCreateContext(dpy, cfg, shareContext->ctx, contextProps.properties());
        if (ctx == EGL_NO_CONTEXT) {
            qWarning() << "QEglContext::createContext(): Could not share context:" << errorString(eglGetError());
            shareContext = 0;
        }
    }
    if (ctx == EGL_NO_CONTEXT) {
        ctx = eglCreateContext(dpy, cfg, 0, contextProps.properties());
        if (ctx == EGL_NO_CONTEXT) {
            qWarning() << "QEglContext::createContext(): Unable to create EGL context:" << errorString(eglGetError());
            return false;
        }
    }
    share = (shareContext != 0);
    return true;
}

// Recreate the surface for a paint device because the native id has changed.
bool QEglContext::recreateSurface(QPaintDevice *device)
{
    // Bail out if the surface has not been created for the first time yet.
    if (surf == EGL_NO_SURFACE)
        return true;

    // Destroy the old surface.
    eglDestroySurface(dpy, surf);

    // Create a new one.
    return createSurface(device);
}

void QEglContext::destroy()
{
    if (ctx != EGL_NO_CONTEXT)
        eglDestroyContext(dpy, ctx);
    dpy = EGL_NO_DISPLAY;
    ctx = EGL_NO_CONTEXT;
    surf = EGL_NO_SURFACE;
    cfg = 0;
    share = false;
}

bool QEglContext::makeCurrent()
{
    if(ctx == EGL_NO_CONTEXT) {
        qWarning() << "QEglContext::makeCurrent(): Cannot make invalid context current";
        return false;
    }

    bool ok = eglMakeCurrent(dpy, surf, surf, ctx);
    if (!ok) {
        EGLint err = eglGetError();
        qWarning() << "QEglContext::makeCurrent():" << errorString(err);
    }
    return ok;
}

bool QEglContext::doneCurrent()
{
    // If the context is invalid, we assume that an error was reported
    // when makeCurrent() was called.
    if (ctx == EGL_NO_CONTEXT)
        return false;

    // We need to select the correct API before calling eglMakeCurrent()
    // with EGL_NO_CONTEXT because threads can have both OpenGL and OpenVG
    // contexts active at the same time.
#ifdef EGL_OPENGL_ES_API
    if (apiType == OpenGL)
        eglBindAPI(EGL_OPENGL_ES_API);
#endif
#ifdef EGL_OPENVG_API
    if (apiType == OpenVG)
        eglBindAPI(EGL_OPENVG_API);
#endif

    bool ok = eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (!ok) {
        EGLint err = eglGetError();
        qWarning() << "QEglContext::doneCurrent():" << errorString(err);
    }
    return ok;
}

bool QEglContext::swapBuffers()
{
    if(ctx == EGL_NO_CONTEXT)
        return false;

    bool ok = eglSwapBuffers(dpy, surf);
    if (!ok) {
        EGLint err = eglGetError();
        qWarning() << "QEglContext::swapBuffers():" << errorString(err);
    }
    return ok;
}

// Wait for native rendering operations to complete before starting
// to use OpenGL/OpenVG operations.
void QEglContext::waitNative()
{
#ifdef EGL_CORE_NATIVE_ENGINE
    eglWaitNative(EGL_CORE_NATIVE_ENGINE);
#endif
}

// Wait for client OpenGL/OpenVG operations to complete before
// using native rendering operations.
void QEglContext::waitClient()
{
#ifdef EGL_OPENGL_ES_API
    if (apiType == OpenGL) {
        eglBindAPI(EGL_OPENGL_ES_API);
        eglWaitClient();
    }
#else
    if (apiType == OpenGL)
        eglWaitGL();
#endif
#ifdef EGL_OPENVG_API
    if (apiType == OpenVG) {
        eglBindAPI(EGL_OPENVG_API);
        eglWaitClient();
    }
#endif
}

// Query the actual size of the EGL surface.
QSize QEglContext::surfaceSize() const
{
    int w, h;
    eglQuerySurface(dpy, surf, EGL_WIDTH, &w);
    eglQuerySurface(dpy, surf, EGL_HEIGHT, &h);
    return QSize(w, h);
}

// Query the value of a configuration attribute.
bool QEglContext::configAttrib(int name, EGLint *value) const
{
    return eglGetConfigAttrib(dpy, cfg, name, value);
}

// Retrieve all of the properties on "cfg".  If zero, return
// the context's configuration.
QEglProperties QEglContext::configProperties(EGLConfig cfg) const
{
    if (!cfg)
        cfg = config();
    QEglProperties props;
    for (int name = 0x3020; name <= 0x304F; ++name) {
        EGLint value;
        if (name != EGL_NONE && eglGetConfigAttrib(dpy, cfg, name, &value))
            props.setValue(name, value);
    }
    eglGetError();  // Clear the error state.
    return props;
}

// Initialize and return the default display.
EGLDisplay QEglContext::defaultDisplay(QPaintDevice *device)
{
    static EGLDisplay dpy = EGL_NO_DISPLAY;
    if (dpy == EGL_NO_DISPLAY) {
        dpy = getDisplay(device);
        if (dpy == EGL_NO_DISPLAY) {
            qWarning() << "QEglContext::defaultDisplay(): Cannot open EGL display";
            return EGL_NO_DISPLAY;
        }
        if (!eglInitialize(dpy, NULL, NULL)) {
            EGLint err = eglGetError();
            qWarning() << "QEglContext::defaultDisplay(): Cannot initialize EGL display:" << errorString(err);
            return EGL_NO_DISPLAY;
        }
#ifdef EGL_OPENGL_ES_API
        eglBindAPI(EGL_OPENGL_ES_API);
#endif
    }
    return dpy;
}

// Return the error string associated with a specific code.
QString QEglContext::errorString(int code)
{
    static const char * const errors[] = {
        "Success (0x3000)",                 // No tr
        "Not initialized (0x3001)",         // No tr
        "Bad access (0x3002)",              // No tr
        "Bad alloc (0x3003)",               // No tr
        "Bad attribute (0x3004)",           // No tr
        "Bad config (0x3005)",              // No tr
        "Bad context (0x3006)",             // No tr
        "Bad current surface (0x3007)",     // No tr
        "Bad display (0x3008)",             // No tr
        "Bad match (0x3009)",               // No tr
        "Bad native pixmap (0x300A)",       // No tr
        "Bad native window (0x300B)",       // No tr
        "Bad parameter (0x300C)",           // No tr
        "Bad surface (0x300D)",             // No tr
        "Context lost (0x300E)"             // No tr
    };
    if (code >= 0x3000 && code <= 0x300E) {
        return QString::fromLatin1(errors[code - 0x3000]);
    } else {
        return QLatin1String("0x") + QString::number(code, 16);
    }
}

// Dump all of the EGL configurations supported by the system.
void QEglContext::dumpAllConfigs()
{
    QEglProperties props;
    EGLint count = 0;
    if (!eglGetConfigs(dpy, 0, 0, &count))
        return;
    if (count < 1)
        return;
    EGLConfig *configs = new EGLConfig [count];
    eglGetConfigs(dpy, configs, count, &count);
    for (EGLint index = 0; index < count; ++index) {
        props = configProperties(configs[index]);
        qWarning() << props.toString();
    }
    delete [] configs;
}

// Initialize a property block.
QEglProperties::QEglProperties()
{
    props.append(EGL_NONE);
}

// Fetch the current value associated with a property.
int QEglProperties::value(int name) const
{
    for (int index = 0; index < (props.size() - 1); index += 2) {
        if (props[index] == name)
            return props[index + 1];
    }
    return EGL_DONT_CARE;
}

// Set the value associated with a property, replacing an existing
// value if there is one.
void QEglProperties::setValue(int name, int value)
{
    for (int index = 0; index < (props.size() - 1); index += 2) {
        if (props[index] == name) {
            props[index + 1] = value;
            return;
        }
    }
    props[props.size() - 1] = name;
    props.append(value);
    props.append(EGL_NONE);
}

// Remove a property value.  Returns false if the property is not present.
bool QEglProperties::removeValue(int name)
{
    for (int index = 0; index < (props.size() - 1); index += 2) {
        if (props[index] == name) {
            while ((index + 2) < props.size()) {
                props[index] = props[index + 2];
                ++index;
            }
            props.resize(props.size() - 2);
            return true;
        }
    }
    return false;
}

// Sets the red, green, blue, and alpha sizes based on a pixel format.
// Normally used to match a configuration request to the screen format.
void QEglProperties::setPixelFormat(QImage::Format pixelFormat)
{
    int red, green, blue, alpha;
    switch (pixelFormat) {
        case QImage::Format_RGB32:
        case QImage::Format_RGB888:
            red = green = blue = 8; alpha = 0; break;
        case QImage::Format_ARGB32:
        case QImage::Format_ARGB32_Premultiplied:
            red = green = blue = alpha = 8; break;
        case QImage::Format_RGB16:
            red = 5; green = 6; blue = 5; alpha = 0; break;
        case QImage::Format_ARGB8565_Premultiplied:
            red = 5; green = 6; blue = 5; alpha = 8; break;
        case QImage::Format_RGB666:
            red = green = blue = 6; alpha = 0; break;
        case QImage::Format_ARGB6666_Premultiplied:
            red = green = blue = alpha = 6; break;
        case QImage::Format_RGB555:
            red = green = blue = 5; alpha = 0; break;
        case QImage::Format_ARGB8555_Premultiplied:
            red = green = blue = 5; alpha = 8; break;
        case QImage::Format_RGB444:
            red = green = blue = 4; alpha = 0; break;
        case QImage::Format_ARGB4444_Premultiplied:
            red = green = blue = alpha = 4; break;
        default:
            qWarning() << "QEglProperties::setPixelFormat(): Unsupported pixel format";
            red = green = blue = alpha = 1; break;
    }
    setValue(EGL_RED_SIZE, red);
    setValue(EGL_GREEN_SIZE, green);
    setValue(EGL_BLUE_SIZE, blue);
    setValue(EGL_ALPHA_SIZE, alpha);
}

void QEglProperties::setRenderableType(int api)
{
#if defined(EGL_RENDERABLE_TYPE)
#if defined(QT_OPENGL_ES_2)
    if (api == QEglContext::OpenGL)
        setValue(EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT);
#elif defined(QT_OPENGL_ES)
    if (api == QEglContext::OpenGL)
        setValue(EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT);
#endif
#if defined(EGL_OPENVG_BIT)
    if (api == QEglContext::OpenVG)
        setValue(EGL_RENDERABLE_TYPE, EGL_OPENVG_BIT);
#endif
#else
    Q_UNUSED(api);
#endif
}

// Reduce the complexity of a configuration request to ask for less
// because the previous request did not result in success.  Returns
// true if the complexity was reduced, or false if no further
// reductions in complexity are possible.
bool QEglProperties::reduceConfiguration()
{
    if (removeValue(EGL_SAMPLE_BUFFERS)) {
        removeValue(EGL_SAMPLES);
        return true;
    }
    if (removeValue(EGL_ALPHA_SIZE))
        return true;
    if (removeValue(EGL_STENCIL_SIZE))
        return true;
    if (removeValue(EGL_DEPTH_SIZE))
        return true;
    return false;
}

static void addTag(QString& str, const QString& tag)
{
    int lastnl = str.lastIndexOf(QLatin1String("\n"));
    if (lastnl == -1)
        lastnl = 0;
    if ((str.length() - lastnl) >= 50)
        str += QLatin1String("\n   ");
    str += tag;
}

// Convert a property list to a string suitable for debug output.
QString QEglProperties::toString() const
{
    QString str;
    int val;

    val = value(EGL_CONFIG_ID);
    if (val != EGL_DONT_CARE) {
        str += QLatin1String("id=");
        str += QString::number(val);
        str += QLatin1String(" ");
    }

#ifdef EGL_RENDERABLE_TYPE
    val = value(EGL_RENDERABLE_TYPE);
    if (val != EGL_DONT_CARE) {
        str += QLatin1String("type=");
        QStringList types;
        if ((val & EGL_OPENGL_ES_BIT) != 0)
            types += QLatin1String("es1");
#ifdef EGL_OPENGL_ES2_BIT
        if ((val & EGL_OPENGL_ES2_BIT) != 0)
            types += QLatin1String("es2");
#endif
        if ((val & EGL_OPENVG_BIT) != 0)
            types += QLatin1String("vg");
        if ((val & ~7) != 0)
            types += QString::number(val);
        str += types.join(QLatin1String(","));
    } else {
        str += QLatin1String("type=any");
    }
#else
    str += QLatin1String("type=es1");
#endif

    int red = value(EGL_RED_SIZE);
    int green = value(EGL_GREEN_SIZE);
    int blue = value(EGL_BLUE_SIZE);
    int alpha = value(EGL_ALPHA_SIZE);
    int bufferSize = value(EGL_BUFFER_SIZE);
    if (bufferSize == (red + green + blue + alpha))
        bufferSize = EGL_DONT_CARE;
    str += QLatin1String(" rgba=");
    str += QString::number(red);
    str += QLatin1String(",");
    str += QString::number(green);
    str += QLatin1String(",");
    str += QString::number(blue);
    str += QLatin1String(",");
    str += QString::number(alpha);
    if (bufferSize != EGL_DONT_CARE) {
        // Only report buffer size if different than r+g+b+a.
        str += QLatin1String(" buffer-size=");
        str += QString::number(bufferSize);
    }

#ifdef EGL_COLOR_BUFFER_TYPE
    val = value(EGL_COLOR_BUFFER_TYPE);
    if (val == EGL_LUMINANCE_BUFFER) {
        addTag(str, QLatin1String(" color-buffer-type=luminance"));
    } else if (val != EGL_DONT_CARE && val != EGL_RGB_BUFFER) {
        addTag(str, QLatin1String(" color-buffer-type="));
        str += QString::number(val, 16);
    }
#endif

    val = value(EGL_DEPTH_SIZE);
    if (val != EGL_DONT_CARE) {
        addTag(str, QLatin1String(" depth="));
        str += QString::number(val);
    }

    val = value(EGL_STENCIL_SIZE);
    if (val != EGL_DONT_CARE) {
        addTag(str, QLatin1String(" stencil="));
        str += QString::number(val);
    }

    val = value(EGL_SURFACE_TYPE);
    if (val != EGL_DONT_CARE) {
        addTag(str, QLatin1String(" surface-type="));
        QStringList types;
        if ((val & EGL_WINDOW_BIT) != 0)
            types += QLatin1String("window");
        if ((val & EGL_PIXMAP_BIT) != 0)
            types += QLatin1String("pixmap");
        if ((val & EGL_PBUFFER_BIT) != 0)
            types += QLatin1String("pbuffer");
#ifdef EGL_VG_COLORSPACE_LINEAR_BIT
        if ((val & EGL_VG_COLORSPACE_LINEAR_BIT) != 0)
            types += QLatin1String("vg-colorspace-linear");
#endif
#ifdef EGL_VG_ALPHA_FORMAT_PRE_BIT
        if ((val & EGL_VG_ALPHA_FORMAT_PRE_BIT) != 0)
            types += QLatin1String("vg-alpha-format-pre");
#endif
        if ((val & ~(EGL_WINDOW_BIT | EGL_PIXMAP_BIT | EGL_PBUFFER_BIT
#ifdef EGL_VG_COLORSPACE_LINEAR_BIT
                     | EGL_VG_COLORSPACE_LINEAR_BIT
#endif
#ifdef EGL_VG_ALPHA_FORMAT_PRE_BIT
                     | EGL_VG_ALPHA_FORMAT_PRE_BIT
#endif
                     )) != 0) {
            types += QString::number(val);
        }
        str += types.join(QLatin1String(","));
    }

    val = value(EGL_CONFIG_CAVEAT);
    if (val != EGL_DONT_CARE) {
        addTag(str, QLatin1String(" caveat="));
        if (val == EGL_NONE)
            str += QLatin1String("none");
        else if (val == EGL_SLOW_CONFIG)
            str += QLatin1String("slow");
        else if (val == EGL_NON_CONFORMANT_CONFIG)
            str += QLatin1String("non-conformant");
        else
            str += QString::number(val, 16);
    }

    val = value(EGL_LEVEL);
    if (val != EGL_DONT_CARE) {
        addTag(str, QLatin1String(" level="));
        str += QString::number(val);
    }

    int width, height, pixels;
    width = value(EGL_MAX_PBUFFER_WIDTH);
    height = value(EGL_MAX_PBUFFER_HEIGHT);
    pixels = value(EGL_MAX_PBUFFER_PIXELS);
    if (height != EGL_DONT_CARE || width != EGL_DONT_CARE) {
        addTag(str, QLatin1String(" max-pbuffer-size="));
        str += QString::number(width);
        str += QLatin1String("x");
        str += QString::number(height);
        if (pixels != (width * height)) {
            addTag(str, QLatin1String(" max-pbuffer-pixels="));
            str += QString::number(pixels);
        }
    }

    val = value(EGL_NATIVE_RENDERABLE);
    if (val != EGL_DONT_CARE) {
        if (val)
            addTag(str, QLatin1String(" native-renderable=true"));
        else
            addTag(str, QLatin1String(" native-renderable=false"));
    }

    val = value(EGL_NATIVE_VISUAL_ID);
    if (val != EGL_DONT_CARE) {
        addTag(str, QLatin1String(" visual-id="));
        str += QString::number(val);
    }

    val = value(EGL_NATIVE_VISUAL_TYPE);
    if (val != EGL_DONT_CARE) {
        addTag(str, QLatin1String(" visual-type="));
        str += QString::number(val);
    }

#ifdef EGL_PRESERVED_RESOURCES
    val = value(EGL_PRESERVED_RESOURCES);
    if (val != EGL_DONT_CARE) {
        if (val)
            addTag(str, QLatin1String(" preserved-resources=true"));
        else
            addTag(str, QLatin1String(" preserved-resources=false"));
    }
#endif

    val = value(EGL_SAMPLES);
    if (val != EGL_DONT_CARE) {
        addTag(str, QLatin1String(" samples="));
        str += QString::number(val);
    }

    val = value(EGL_SAMPLE_BUFFERS);
    if (val != EGL_DONT_CARE) {
        addTag(str, QLatin1String(" sample-buffers="));
        str += QString::number(val);
    }

    val = value(EGL_TRANSPARENT_TYPE);
    if (val == EGL_TRANSPARENT_RGB) {
        addTag(str, QLatin1String(" transparent-rgb="));
        str += QString::number(value(EGL_TRANSPARENT_RED_VALUE));
        str += QLatin1String(",");
        str += QString::number(value(EGL_TRANSPARENT_GREEN_VALUE));
        str += QLatin1String(",");
        str += QString::number(value(EGL_TRANSPARENT_BLUE_VALUE));
    }

#if defined(EGL_BIND_TO_TEXTURE_RGB) && defined(EGL_BIND_TO_TEXTURE_RGBA)
    val = value(EGL_BIND_TO_TEXTURE_RGB);
    int val2 = value(EGL_BIND_TO_TEXTURE_RGBA);
    if (val != EGL_DONT_CARE || val2 != EGL_DONT_CARE) {
        addTag(str, QLatin1String(" bind-texture="));
        if (val == EGL_TRUE)
            str += QLatin1String("rgb");
        else
            str += QLatin1String("no-rgb");
        if (val2 == EGL_TRUE)
            str += QLatin1String(",rgba");
        else
            str += QLatin1String(",no-rgba");
    }
#endif

#ifdef EGL_MIN_SWAP_INTERVAL
    val = value(EGL_MIN_SWAP_INTERVAL);
    if (val != EGL_DONT_CARE) {
        addTag(str, QLatin1String(" min-swap-interval="));
        str += QString::number(val);
    }
#endif

#ifdef EGL_MIN_SWAP_INTERVAL
    val = value(EGL_MAX_SWAP_INTERVAL);
    if (val != EGL_DONT_CARE) {
        addTag(str, QLatin1String(" max-swap-interval="));
        str += QString::number(val);
    }
#endif

#ifdef EGL_LUMINANCE_SIZE
    val = value(EGL_LUMINANCE_SIZE);
    if (val != EGL_DONT_CARE) {
        addTag(str, QLatin1String(" luminance="));
        str += QString::number(val);
    }
#endif

#ifdef EGL_ALPHA_MASK_SIZE
    val = value(EGL_ALPHA_MASK_SIZE);
    if (val != EGL_DONT_CARE) {
        addTag(str, QLatin1String(" alpha-mask="));
        str += QString::number(val);
    }
#endif

#ifdef EGL_CONFORMANT
    val = value(EGL_CONFORMANT);
    if (val != EGL_DONT_CARE) {
        if (val)
            addTag(str, QLatin1String(" conformant=true"));
        else
            addTag(str, QLatin1String(" conformant=false"));
    }
#endif

    return str;
}

QT_END_NAMESPACE

#endif // QT_OPENGL_ES || QT_OPENVG
