/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QPDF_P_H
#define QPDF_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//
#include "QtGui/qmatrix.h"
#include "QtCore/qstring.h"
#include "QtCore/qvector.h"
#include "private/qstroker_p.h"
#include "private/qfontengine_p.h"
#include "QtGui/qprinter.h"
#include "private/qfontsubset_p.h"
#include "private/qpaintengine_p.h"
#include "qprintengine.h"

#ifndef QT_NO_PRINTER

QT_BEGIN_NAMESPACE

#define PPK_CupsOptions QPrintEngine::PrintEnginePropertyKey(0xfe00)
#define PPK_CupsPageRect QPrintEngine::PrintEnginePropertyKey(0xfe01)
#define PPK_CupsPaperRect QPrintEngine::PrintEnginePropertyKey(0xfe02)
#define PPK_CupsStringPageSize QPrintEngine::PrintEnginePropertyKey(0xfe03)

const char *qt_real_to_string(qreal val, char *buf);
const char *qt_int_to_string(int val, char *buf);

namespace QPdf {

    class ByteStream
    {
    public:
        ByteStream(QByteArray *b) :ba(b) {}
        ByteStream &operator <<(char chr) { *ba += chr; return *this; }
        ByteStream &operator <<(const char *str) { *ba += str; return *this; }
        ByteStream &operator <<(const QByteArray &str) { *ba += str; return *this; }
        ByteStream &operator <<(qreal val) { char buf[256]; *ba += qt_real_to_string(val, buf); return *this; }
        ByteStream &operator <<(int val) { char buf[256]; *ba += qt_int_to_string(val, buf); return *this; }
        ByteStream &operator <<(const QPointF &p) { char buf[256]; *ba += qt_real_to_string(p.x(), buf);
            *ba += qt_real_to_string(p.y(), buf); return *this; }
    private:
        QByteArray *ba;
    };

    enum PathFlags {
        ClipPath,
        FillPath,
        StrokePath,
        FillAndStrokePath
    };
    QByteArray generatePath(const QPainterPath &path, const QTransform &matrix, PathFlags flags);
    QByteArray generateMatrix(const QTransform &matrix);
    QByteArray generateDashes(const QPen &pen);
    QByteArray patternForBrush(const QBrush &b);
#ifdef USE_NATIVE_GRADIENTS
    QByteArray generateLinearGradientShader(const QLinearGradient *lg, const QPointF *page_rect, bool alpha = false);
#endif

    struct Stroker {
        Stroker();
        void setPen(const QPen &pen);
        void strokePath(const QPainterPath &path);
        ByteStream *stream;
        bool first;
        QTransform matrix;
        bool cosmeticPen;
    private:
        QStroker basicStroker;
        QDashStroker dashStroker;
        QStrokerOps *stroker;
    };

    QByteArray ascii85Encode(const QByteArray &input);

    const char *toHex(ushort u, char *buffer);
    const char *toHex(uchar u, char *buffer);


    struct PaperSize {
        int width, height; // in postscript points
    };
    PaperSize paperSize(QPrinter::PaperSize paperSize);
    const char *paperSizeToString(QPrinter::PaperSize paperSize);


    QByteArray stripSpecialCharacters(const QByteArray &string);
};


class QPdfPage : public QPdf::ByteStream
{
public:
    QPdfPage();
    QByteArray content() { return data; }

    QVector<uint> images;
    QVector<uint> graphicStates;
    QVector<uint> patterns;
    QVector<uint> fonts;
    QVector<uint> annotations;

    void streamImage(int w, int h, int object);

    QSize pageSize;
private:
    QByteArray data;
};


class QPdfBaseEnginePrivate;

class QPdfBaseEngine : public QPaintEngine, public QPrintEngine
{
    Q_DECLARE_PRIVATE(QPdfBaseEngine)
public:
    QPdfBaseEngine(QPdfBaseEnginePrivate &d, PaintEngineFeatures f);
    ~QPdfBaseEngine() {}

    // reimplementations QPaintEngine
    bool begin(QPaintDevice *pdev);
    bool end();

    void drawPoints(const QPointF *points, int pointCount);
    void drawLines(const QLineF *lines, int lineCount);
    void drawRects(const QRectF *rects, int rectCount);
    void drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode);
    void drawPath (const QPainterPath & path);

    void drawTextItem(const QPointF &p, const QTextItem &textItem);

    void updateState(const QPaintEngineState &state);

    int metric(QPaintDevice::PaintDeviceMetric metricType) const;
    // end reimplementations QPaintEngine

    // Printer stuff...
    bool newPage();
    void setProperty(PrintEnginePropertyKey key, const QVariant &value);
    QVariant property(PrintEnginePropertyKey key) const;

    void setPen();
    virtual void setBrush() = 0;
    void setupGraphicsState(QPaintEngine::DirtyFlags flags);

private:
    void updateClipPath(const QPainterPath & path, Qt::ClipOperation op);
};

class QPdfBaseEnginePrivate : public QPaintEnginePrivate
{
    Q_DECLARE_PUBLIC(QPdfBaseEngine)
public:
    QPdfBaseEnginePrivate(QPrinter::PrinterMode m);
    ~QPdfBaseEnginePrivate();

    bool openPrintDevice();
    void closePrintDevice();


    virtual void drawTextItem(const QPointF &p, const QTextItemInt &ti);
    inline uint requestObject() { return currentObject++; }

    QRect paperRect() const;
    QRect pageRect() const;

    bool postscript;
    int currentObject;

    QPdfPage* currentPage;
    QPdf::Stroker stroker;

    QPointF brushOrigin;
    QBrush brush;
    QPen pen;
    QList<QPainterPath> clips;
    bool clipEnabled;
    bool allClipped;
    bool hasPen;
    bool hasBrush;
    bool simplePen;
    qreal opacity;

    QHash<QFontEngine::FaceId, QFontSubset *> fonts;

    QPaintDevice *pdev;

    // the device the output is in the end streamed to.
    QIODevice *outDevice;
    int fd;

    // printer options
    QString outputFileName;
    QString printerName;
    QString printProgram;
    QString selectionOption;
    QString title;
    QString creator;
    QPrinter::DuplexMode duplex;
    bool collate;
    bool fullPage;
    bool embedFonts;
    int copies;
    int resolution;
    QPrinter::PageOrder pageOrder;
    QPrinter::Orientation orientation;
    QPrinter::PaperSize paperSize;
    QPrinter::ColorMode colorMode;
    QPrinter::PaperSource paperSource;

    QStringList cupsOptions;
    QRect cupsPaperRect;
    QRect cupsPageRect;
    QString cupsStringPageSize;
    QSizeF customPaperSize; // in postscript points
    bool hasCustomPageMargins;
    qreal leftMargin, topMargin, rightMargin, bottomMargin;

#if !defined(QT_NO_CUPS) && !defined(QT_NO_LIBRARY)
    QString cupsTempFile;
#endif
};

QT_END_NAMESPACE

#endif // QT_NO_PRINTER

#endif // QPDF_P_H

