/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "private/qpicture_p.h"
#include "qfile.h"
#include "qpainter.h"
#include "q3picture.h"
#include "q3paintengine_svg_p.h"

QT_BEGIN_NAMESPACE

class Q3SvgDevice : public QPaintDevice
{
public:
    Q3SvgDevice() : QPaintDevice() {}
    bool load(QIODevice *dev) { return svgEngine.load(dev); }
    bool save(const QString &fileName) { return svgEngine.save(fileName); }
    bool save(QIODevice *dev) { return svgEngine.save(dev); }
    void setBoundingRect(const QRect &rect) { svgEngine.setBoundingRect(rect); }
    QRect boundingRect() const { return svgEngine.boundingRect(); }
    QPaintEngine *paintEngine() const { return (QPaintEngine *)&svgEngine; }
    bool play(QPainter *p) { return svgEngine.play(p); }
    int metric(PaintDeviceMetric m) const;

private:
    Q3SVGPaintEngine svgEngine;
};

int Q3SvgDevice::metric(PaintDeviceMetric m) const
{
        int val;
        QRect br = svgEngine.boundingRect();
        switch (m) {
        case PdmWidth:
            val = br.width();
            break;
        case PdmHeight:
            val = br.height();
            break;
        case PdmWidthMM:
            val = int(25.4/72.0*br.width());
            break;
        case PdmHeightMM:
            val = int(25.4/72.0*br.height());
            break;
        case PdmDpiX:
            val = 72;
            break;
        case PdmDpiY:
            val = 72;
            break;
        case PdmNumColors:
            val = 16777216;
            break;
        case PdmDepth:
            val = 24;
            break;
        default:
            val = 0;
            qWarning("Q3SvgDevice::metric: Invalid metric command");
        }
        return val;
}

/*!
    \class Q3Picture
    \brief The Q3Picture class is a paint device that records and
    replays Q3Painter commands.

    \compat

    Q3Picture can also read and write SVG (Scalable Vector Graphics)
    files; these files are in an XML format specified by \l {
    http://www.w3.org/Graphics/SVG/ W3C}. Note that when using the
    load() and save() functions to read and write SVG files, the
    format must be specified.

    \sa QPicture
*/

/*!
    \fn Q3Picture::Q3Picture()

    Constructs a Q3Picture.
*/

/*!
    \fn Q3Picture::Q3Picture(const QPicture &other)

    Constructs a copy of \a other.
*/

/*!
    \overload
    Loads the picture in the specified \a format from a file with the
    given \a fileName. Returns true if the file is loaded successfully;
    otherwise returns false.
*/
bool Q3Picture::load(const QString &fileName, const char *format)
{
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly))
        return false;
    return load(&f, format);
}

/*!
    \fn bool Q3Picture::load(QIODevice *device, const char *format)

    Loads the picture in the specified \a format from the given \a device.
    Returns true if the file is loaded successfully; otherwise returns false.

    Note that when using the load() function to read SVG files, the
    format must be specified. For example:

    \snippet doc/src/snippets/code/src.qt3support.painting.q3picture.cpp 0

    \sa save()
*/
bool Q3Picture::load(QIODevice *dev, const char *format)
{
    if (qstrcmp(format, "svg" ) == 0) {
	Q3SvgDevice svg;
	if (!svg.load(dev))
	    return false;
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
	bool b = svg.play(&p);
	d_func()->brect = svg.boundingRect();
	return b;
    }
    return QPicture::load(dev, format);
}

/*!
    \overload
    Saves the picture in the specified \a format to the file with the
    given \a fileName.

    \sa load()
*/
bool Q3Picture::save(const QString &fileName, const char *format)
{
    if (paintingActive()) {
        qWarning("Q3Picture::save: still being painted on. "
                  "Call QPainter::end() first");
        return false;
    }

    // identical to QIODevice* code below but the file name
    // makes a difference when it comes to saving pixmaps
    if (qstricmp( format, "svg") == 0) {
	Q3SvgDevice svg;
	QPainter p(&svg);
	if (!play(&p))
	    return false;
	svg.setBoundingRect(boundingRect());
	return svg.save(fileName);
    }

    return QPicture::save(fileName, format);
}

/*!
    \fn bool Q3Picture::save(QIODevice *device, const char *format)

    Saves the picture in the specified \a format to the given \a device.
    Returns true if the save is successful. Returns false if, for
    example, the picture is still being painted, i.e., QPainter::end()
    has not yet been called.

    Note that when using the save() function to save SVG files, the
    format must be specified. For example:

    \snippet doc/src/snippets/code/src.qt3support.painting.q3picture.cpp 1

    \sa load()
*/
bool Q3Picture::save(QIODevice *dev, const char *format)
{
    if (paintingActive()) {
        qWarning("Q3Picture::save: still being painted on. "
                  "Call QPainter::end() first");
        return false;
    }

    if (qstricmp(format, "svg") == 0) {
	Q3SvgDevice svg;
	QPainter p(&svg);
	if (!play(&p))
	    return false;
	svg.setBoundingRect(boundingRect());
	return svg.save(dev);
    }
    return QPicture::save(dev, format);
}

QT_END_NAMESPACE
