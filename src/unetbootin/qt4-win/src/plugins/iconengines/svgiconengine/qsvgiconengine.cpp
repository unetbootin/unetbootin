/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the plugins of the Qt Toolkit.
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
#include "qsvgiconengine.h"

#ifndef QT_NO_SVGRENDERER

#include "qpainter.h"
#include "qpixmap.h"
#include "qsvgrenderer.h"
#include "qpixmapcache.h"
#include "qstyle.h"
#include "qapplication.h"
#include "qstyleoption.h"
#include "qfileinfo.h"
#include <QAtomicInt>
#include "qdebug.h"

QT_BEGIN_NAMESPACE

class QSvgIconEnginePrivate : public QSharedData
{
public:
    QSvgIconEnginePrivate()
        : svgBuffers(0), addedPixmaps(0)
        { stepSerialNum(); }

    ~QSvgIconEnginePrivate()
        { delete addedPixmaps; delete svgBuffers; }

    static int hashKey(QIcon::Mode mode, QIcon::State state)
        { return (((mode)<<4)|state); }

    QString pmcKey(const QSize &size, QIcon::Mode mode, QIcon::State state)
        { return QLatin1String("$qt_svgicon_")
                 + QString::number(serialNum, 16).append(QLatin1Char('_'))
                 + QString::number((((((size.width()<<11)|size.height())<<11)|mode)<<4)|state, 16); }

    void stepSerialNum()
        { serialNum = lastSerialNum.fetchAndAddRelaxed(1); };

    QHash<int, QString> svgFiles;
    QHash<int, QByteArray> *svgBuffers;
    QHash<int, QPixmap> *addedPixmaps;
    int serialNum;
    static QAtomicInt lastSerialNum;
};

QAtomicInt QSvgIconEnginePrivate::lastSerialNum;

static inline int pmKey(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    return ((((((size.width()<<11)|size.height())<<11)|mode)<<4)|state);
}

QSvgIconEngine::QSvgIconEngine()
    : d(new QSvgIconEnginePrivate)
{
}

QSvgIconEngine::QSvgIconEngine(const QSvgIconEngine &other)
    : QIconEngineV2(other), d(new QSvgIconEnginePrivate)
{
    d->svgFiles = other.d->svgFiles;
    if (other.d->svgBuffers)
        d->svgBuffers = new QHash<int, QByteArray>(*other.d->svgBuffers);
    if (other.d->addedPixmaps)
        d->addedPixmaps = new QHash<int, QPixmap>(*other.d->addedPixmaps);
}


QSvgIconEngine::~QSvgIconEngine()
{
}


QSize QSvgIconEngine::actualSize(const QSize &size, QIcon::Mode,
                                 QIcon::State )
{
    return size;
}


QPixmap QSvgIconEngine::pixmap(const QSize &size, QIcon::Mode mode,
                               QIcon::State state)
{
    QPixmap pm;
    QString pmckey(d->pmcKey(size, mode, state));
    if (QPixmapCache::find(pmckey, pm))     // Note: OUT parameter
        return pm;

    if (d->addedPixmaps) {
        pm = d->addedPixmaps->value(d->hashKey(mode, state));
        if (!pm.isNull() && pm.size() != size)
                pm = pm.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    if (pm.isNull()) {
        QSvgRenderer renderer;
        QByteArray buf;
        if (d->svgBuffers) {
            buf = d->svgBuffers->value(d->hashKey(mode, state));
            if (buf.isEmpty())
                buf = d->svgBuffers->value(d->hashKey(QIcon::Normal, QIcon::Off));
        }
        if (!buf.isEmpty()) {
#ifndef QT_NO_COMPRESS
            buf = qUncompress(buf);
#endif
            renderer.load(buf);
        }
        else {
            QString svgFile = d->svgFiles.value(d->hashKey(mode, state));
            if (svgFile.isEmpty())
                svgFile = d->svgFiles.value(d->hashKey(QIcon::Normal, QIcon::Off));
            if (!svgFile.isEmpty())
                renderer.load(svgFile);
        }
        if (renderer.isValid()) {
            QImage img(size, QImage::Format_ARGB32_Premultiplied);
            img.fill(0x00000000);
            QPainter p(&img);
            renderer.render(&p);
            p.end();
            pm = QPixmap::fromImage(img);
            QStyleOption opt(0);
            opt.palette = QApplication::palette();
            QPixmap generated = QApplication::style()->generatedIconPixmap(mode, pm, &opt);
            if (!generated.isNull())
                pm = generated;
        }
    }

    if (!pm.isNull())
        QPixmapCache::insert(pmckey, pm);

    return pm;
}


void QSvgIconEngine::addPixmap(const QPixmap &pixmap, QIcon::Mode mode,
                               QIcon::State state)
{
    if (!d->addedPixmaps)
        d->addedPixmaps = new QHash<int, QPixmap>;
    d->stepSerialNum();
    d->addedPixmaps->insert(d->hashKey(mode, state), pixmap);
}


void QSvgIconEngine::addFile(const QString &fileName, const QSize &,
                             QIcon::Mode mode, QIcon::State state)
{
    if (!fileName.isEmpty()) {
        QString abs = fileName;
        if (fileName.at(0) != QLatin1Char(':'))
            abs = QFileInfo(fileName).absoluteFilePath();
        if (abs.endsWith(key())) {
            QSvgRenderer renderer(abs);
            if (renderer.isValid()) {
                d->stepSerialNum();
                d->svgFiles.insert(d->hashKey(mode, state), abs);
            }
        } else {
            QPixmap pm(abs);
            if (!pm.isNull())
                addPixmap(pm, mode, state);
        }
    }
}

void QSvgIconEngine::paint(QPainter *painter, const QRect &rect,
                           QIcon::Mode mode, QIcon::State state)
{
    painter->drawPixmap(rect, pixmap(rect.size(), mode, state));
}

QString QSvgIconEngine::key() const
{
    return QLatin1String("svg");
}

QIconEngineV2 *QSvgIconEngine::clone() const
{
    return new QSvgIconEngine(*this);
}


bool QSvgIconEngine::read(QDataStream &in)
{
    d = new QSvgIconEnginePrivate;
    d->svgBuffers = new QHash<int, QByteArray>;

    if (in.version() >= QDataStream::Qt_4_4) {
        int isCompressed;
        QHash<int, QString> fileNames;  // For memoryoptimization later
        in >> fileNames >> isCompressed >> *d->svgBuffers;
#ifndef QT_NO_COMPRESS
        if (!isCompressed) {
            foreach(int key, d->svgBuffers->keys())
                d->svgBuffers->insert(key, qCompress(d->svgBuffers->value(key)));
        }
#else
        if (isCompressed) {
            qWarning("QSvgIconEngine: Can not decompress SVG data");
            d->svgBuffers->clear();
        }
#endif
        int hasAddedPixmaps;
        in >> hasAddedPixmaps;
        if (hasAddedPixmaps) {
            d->addedPixmaps = new QHash<int, QPixmap>;
            in >> *d->addedPixmaps;
        }
    }
    else {
        QPixmap pixmap;
        QByteArray data;
        uint mode;
        uint state;
        int num_entries;

        in >> data;
        if (!data.isEmpty()) {
#ifndef QT_NO_COMPRESS
            data = qUncompress(data);
#endif
            if (!data.isEmpty())
                d->svgBuffers->insert(d->hashKey(QIcon::Normal, QIcon::Off), data);
        }
        in >> num_entries;
        for (int i=0; i<num_entries; ++i) {
            if (in.atEnd())
                return false;
            in >> pixmap;
            in >> mode;
            in >> state;
            // The pm list written by 4.3 is buggy and/or useless, so ignore.
            //addPixmap(pixmap, QIcon::Mode(mode), QIcon::State(state));
        }
    }

    return true;
}


bool QSvgIconEngine::write(QDataStream &out) const
{
    if (out.version() >= QDataStream::Qt_4_4) {
        int isCompressed = 0;
#ifndef QT_NO_COMPRESS
        isCompressed = 1;
#endif
        QHash<int, QByteArray> svgBuffers;
        if (d->svgBuffers)
            svgBuffers = *d->svgBuffers;
        foreach(int key, d->svgFiles.keys()) {
            QByteArray buf;
            QFile f(d->svgFiles.value(key));
            if (f.open(QIODevice::ReadOnly))
                buf = f.readAll();
#ifndef QT_NO_COMPRESS
            buf = qCompress(buf);
#endif
            svgBuffers.insert(key, buf);
        }
        out << d->svgFiles << isCompressed << svgBuffers;
        if (d->addedPixmaps)
            out << (int)1 << *d->addedPixmaps;
        else
            out << (int)0;
    }
    else {
        QByteArray buf;
        if (d->svgBuffers)
            buf = d->svgBuffers->value(d->hashKey(QIcon::Normal, QIcon::Off));
        if (buf.isEmpty()) {
            QString svgFile = d->svgFiles.value(d->hashKey(QIcon::Normal, QIcon::Off));
            if (!svgFile.isEmpty()) {
                QFile f(svgFile);
                if (f.open(QIODevice::ReadOnly))
                    buf = f.readAll();
            }
        }
#ifndef QT_NO_COMPRESS
        buf = qCompress(buf);
#endif
        out << buf;
        // 4.3 has buggy handling of added pixmaps, so don't write any
        out << (int)0;
    }
    return true;
}

QT_END_NAMESPACE

#endif // QT_NO_SVGRENDERER
