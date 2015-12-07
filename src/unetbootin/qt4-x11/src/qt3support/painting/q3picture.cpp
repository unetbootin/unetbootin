/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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
    files, a \l{http://www.w3.org/Graphics/SVG/} {W3C} XML format.
    Note that when using the load() and save() functions to read and
    write SVG files, the format must be specified.

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

    \snippet doc/src/snippets/code/src_qt3support_painting_q3picture.cpp 0

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

    \snippet doc/src/snippets/code/src_qt3support_painting_q3picture.cpp 1

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
